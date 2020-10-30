#include "Renderer/RendererModule.h"
#include "Renderer/FrameGraph/RenderPass.h"
#include "Render/RenderSystem/RenderSystem.h"
#include <array>
#include "Threading/JobSystem/JobSystem.h"
#include "Renderer/WorldRenderer.h"

ZE_DEFINE_MODULE(ze::renderer::CRendererModule, Renderer);

namespace ze::renderer
{

const std::array<SQuadVertex, 4> QuadVertices =
{
	SQuadVertex(maths::Vector2f(-1, -1), maths::Vector2f(0, 0)),
	SQuadVertex(maths::Vector2f(1, -1), maths::Vector2f(1, 0)),
	SQuadVertex(maths::Vector2f(1, 1), maths::Vector2f(1, 1)),
	SQuadVertex(maths::Vector2f(-1, 1), maths::Vector2f(0, 1)),
};

const std::array<uint16_t, 6> QuadIndices = 
{
	 0, 1, 2, 
	 2, 3, 0
};

CRendererModule::CRendererModule()
{
	QuadVBuffer = GRenderSystem->CreateBuffer({
		ERSBufferUsageFlagBits::VertexBuffer,
		ERSMemoryUsage::DeviceLocal,
		ERSMemoryHintFlagBits::None,
		QuadVertices.size() * sizeof(QuadVertices.front()) });

	QuadIBuffer = GRenderSystem->CreateBuffer({
		ERSBufferUsageFlagBits::IndexBuffer,
		ERSMemoryUsage::DeviceLocal,
		ERSMemoryHintFlagBits::None,
		QuadIndices.size() * sizeof(QuadIndices.front()) });

	RSUtils::Copy(QuadVertices.data(), QuadVBuffer.get());
	RSUtils::Copy(QuadIndices.data(), QuadIBuffer.get());
}

CRendererModule::~CRendererModule() 
{
	QuadVBuffer.reset();
	QuadIBuffer.reset();
	CRenderPassPersistentResourceManager::Get().Destroy();
}

CRendererModule& CRendererModule::Get()
{
	static CRendererModule& Module = *ze::module::load_module<CRendererModule>("Renderer");
	return Module;
}

void CRendererModule::CreateImGuiRenderer()
{
	//ImGuiRenderer = std::make_unique<ui::CImGuiRender>();
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

	//ImGuiRenderer->CopyDrawdata();

	for(const auto& View : Views)
		BeginDrawView(View);

	Views.clear();
}

void CRendererModule::WaitRendering()
{
	/** Wait any rendering jobs if they are not finished */
	for (const auto& Job : RenderingJobs)
		ze::jobsystem::wait(*Job);

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
	const jobsystem::Job& Job = jobsystem::create_job(jobsystem::JobType::Normal,
		[&](const jobsystem::Job& InJob)
	{
		WorldRenderer->Prepare();
		WorldRenderer->Draw();
	});

	RenderingJobs.emplace_back(&Job);
	jobsystem::schedule(Job);
}

}