#include "Renderer/RendererModule.h"
#include "Renderer/FrameGraph/RenderPass.h"
#include "Render/RenderSystem/RenderSystem.h"
#include <array>
#include "Threading/JobSystem/JobSystem.h"
#include "Renderer/WorldRenderer.h"
#include "ImGui/ImGuiRender.h"

DEFINE_MODULE(ZE::Renderer::CRendererModule, Renderer);

namespace ZE::Renderer
{

const std::array<SQuadVertex, 4> QuadVertices =
{
	SQuadVertex(Math::SVector2f(-1, -1), Math::SVector2f(0, 0)),
	SQuadVertex(Math::SVector2f(1, -1), Math::SVector2f(1, 0)),
	SQuadVertex(Math::SVector2f(1, 1), Math::SVector2f(1, 1)),
	SQuadVertex(Math::SVector2f(-1, 1), Math::SVector2f(0, 1)),
};

const std::array<uint16_t, 6> QuadIndices = 
{
	 0, 1, 2, 
	 2, 3, 0
};

CRendererModule::~CRendererModule() = default;


CRendererModule& CRendererModule::Get()
{
	static CRendererModule& Module = *CModuleManager::LoadModule<CRendererModule>("Renderer");
	return Module;
}

void CRendererModule::Initialize()
{
	QuadVBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::VertexBuffer,
		ERSMemoryUsage::DeviceLocal,
		QuadVertices.size() * sizeof(QuadVertices.front()),
		SRSResourceCreateInfo(QuadVertices.data()));

	QuadIBuffer = GRenderSystem->CreateBuffer(
		ERSBufferUsage::IndexBuffer,
		ERSMemoryUsage::DeviceLocal,
		QuadIndices.size() * sizeof(QuadIndices.front()),
		SRSResourceCreateInfo(QuadIndices.data()));
}

void CRendererModule::CreateImGuiRenderer()
{
	ImGuiRenderer = std::make_unique<UI::CImGuiRender>();
}

void CRendererModule::Destroy()
{
	QuadVBuffer.reset();
	QuadIBuffer.reset();
	CRenderPassPersistentResourceManager::Get().Destroy();
}

void CRendererModule::EnqueueView(const SWorldView& InView)
{
	Views.emplace_back(std::move(InView));
}

void CRendererModule::FlushViews()
{
	WaitRendering();

	TransientFrameDataMap.clear();
	WorldRenderers.clear();
	WorldRenderers.reserve(Views.size());

	ImGuiRenderer->CopyDrawdata();

	for(const auto& View : Views)
		BeginDrawView(View);

	Views.clear();
}

void CRendererModule::WaitRendering()
{
	/** Wait any rendering jobs if they are not finished */
	for (const auto& Job : RenderingJobs)
		ZE::JobSystem::WaitJob(*Job);

	RenderingJobs.clear();
}

void CRendererModule::BeginDrawView(const SWorldView& InView)
{
	/** Start rendering of this view 
	 * Rendering in ZinoEngine is made of 3 phases:
	 * 
	 * - Game state copy phase, this is the phase that fill the transient data structures
	 *	it must be executed as fast as possible to be able to tick frame N + 1
	 * - Preparation phase, this is the phase where all transient data is used to prepare the actual
	 *	rendering, e.g: transformations calculations
	 * - Draw phase phase, this is the phase where the renderer submit drawcalls
	 */

	/** Instantiate a world renderer */
	WorldRenderers.emplace_back(std::make_unique<CWorldRenderer>(InView));
	const auto& WorldRenderer = WorldRenderers.back();
	
	/** Compute a list of visible proxies */
	WorldRenderer->CheckVisibility();

	/** Copy the game state */
	WorldRenderer->CopyGameState(TransientFrameDataMap);

	/** Queue the rendering */
	const JobSystem::SJob& Job = JobSystem::CreateJob(JobSystem::EJobType::Normal,
		[&](const JobSystem::SJob& InJob)
	{
		WorldRenderer->Prepare();
		WorldRenderer->Draw();
	});

	RenderingJobs.emplace_back(&Job);
	JobSystem::ScheduleJob(Job);
}

}