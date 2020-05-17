#include "Engine/EngineGame.h"
#include "Render/Window.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Engine/Viewport.h"
#include "Render/RenderThread.h"
#include <array>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Engine/StaticMesh.h"
#include <SDL2/SDL.h>
#include "Engine/World.h"
#include "Engine/ECS.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/StaticMeshComponent.h"
#include "Renderer/ClusteredForward/ClusteredForwardWorldRenderer.h"

namespace ZE
{

CEngineGame::~CEngineGame() { delete Window; }

void LoadModelUsingAssimp(const std::string_view& InPath,
	std::vector<SStaticMeshVertex>& OutVertices,
	std::vector<uint32_t>& OutIndices)
{
	static std::vector<Math::SVector3Float> Colors = 
	{
		{ { 1, 0, 0 } },
		{ { 0, 1, 0 } },
		{ { 0, 0, 1 } }
	};
	static size_t CurrentColor = 0;
	static size_t CurrentVtx = 0;

	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(InPath.data(), aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);
	if(!Scene)
		LOG(ELogSeverity::Fatal, None, "Failed to load model %s", InPath.data());

	aiMesh* Mesh = Scene->mMeshes[0];

	OutVertices.reserve(Mesh->mNumVertices);
	
	std::unordered_map<SStaticMeshVertex, uint32_t, SStaticMeshVertexHash> UniqueVertices;

	for(uint32_t i = 0; i < Mesh->mNumVertices; ++i)
	{
		aiVector3D& Position = Mesh->mVertices[i];

		SStaticMeshVertex Vertex;
		Vertex.Position = { Position.x, Position.y, Position.z };
		Vertex.Color = Colors[CurrentColor];

		if(UniqueVertices.count(Vertex) == 0)
		{
			UniqueVertices[Vertex] = static_cast<uint32_t>(OutVertices.size());
			CurrentVtx++;
			if(CurrentVtx == 32)
			{
				CurrentColor = (CurrentColor + 1) % Colors.size();
				CurrentVtx = 0;
			}
			OutVertices.push_back(Vertex);
		}

		OutIndices.emplace_back(UniqueVertices[Vertex]);
	}

}

struct test
{
	alignas(16) glm::mat4 View;
	alignas(16) glm::vec3 CameraPos;
	alignas(16) glm::vec3 CameraFront;
	alignas(16) glm::vec3 CameraUp;
};

void CEngineGame::Initialize()
{
	CEngine::Initialize();

	using namespace ZE::Components;

	/** Create main game window */
	Window = new CWindow("ZinoEngine", 1280, 720);
	Viewport = std::make_unique<CViewport>(Window->GetHandle(), 1280, 720);
	Viewport->InitResource();

	World = std::make_unique<CWorld>();

	ECS::EntityID Test = World->GetEntityManager()->CreateEntity();
	ECS::EntityID Test2 = World->GetEntityManager()->CreateEntity();
	ECS::EntityID Test3 = World->GetEntityManager()->CreateEntity();
	ECS::EntityID Test4 = World->GetEntityManager()->CreateEntity();
	ECS::EntityID Test5 = World->GetEntityManager()->CreateEntity();

	World->GetEntityManager()->AddComponent(
		Test, Refl::CStruct::Get<Components::STransformComponent>());
	World->GetEntityManager()->AddComponent(
		Test, Refl::CStruct::Get<Components::SStaticMeshComponent>());

	World->GetEntityManager()->AddComponent(
		Test, Refl::CStruct::Get<ECS::SHierarchyComponent>());
	World->GetEntityManager()->AddComponent(
		Test2, Refl::CStruct::Get<ECS::SHierarchyComponent>());
	World->GetEntityManager()->AddComponent(
		Test3, Refl::CStruct::Get<ECS::SHierarchyComponent>());
	World->GetEntityManager()->AddComponent(
		Test4, Refl::CStruct::Get<ECS::SHierarchyComponent>());
	World->GetEntityManager()->AddComponent(
		Test5, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	World->GetEntityManager()->AttachEntity(Test2, Test);
	World->GetEntityManager()->AttachEntity(Test3, Test);
	World->GetEntityManager()->AttachEntity(Test4, Test);
	World->GetEntityManager()->AttachEntity(Test5, Test);
	
	ECS::SEntityComponent* Transform = World->GetEntityManager()->GetComponent(
		Test, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	ECS::SEntityComponent* Hiera2 = World->GetEntityManager()->GetComponent(
		Test2, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	ECS::SEntityComponent* Hiera3 = World->GetEntityManager()->GetComponent(
		Test3, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	ECS::SEntityComponent* Hiera4 = World->GetEntityManager()->GetComponent(
		Test4, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	ECS::SEntityComponent* Hiera5 = World->GetEntityManager()->GetComponent(
		Test5, Refl::CStruct::Get<ECS::SHierarchyComponent>());

	/**
	 * Read merger sponger
	 */
	std::vector<SStaticMeshVertex> Vertices;/* = 
	{
		{ {-1, 1, 0}, {} },
		{ {1, 1, 0}, {} },
		{ {-1, -1, 0}, {} },
		{ {1, -1, 0}, {} },
	};*/
	std::vector<uint32_t> Indices;/*= 
	{ 
		2, 1, 1,
		3, 2, 1,
		1, 3, 1,
		2, 1, 1,
		4, 4, 1,
		3, 2, 1
	};*/
	LoadModelUsingAssimp("Assets/MergerSponge.obj", Vertices, Indices);

	testSM = std::make_unique<CStaticMesh>();
	testSM->UpdateData(Vertices, Indices);

	ubo = GRenderSystem->CreateBuffer(
		ERSBufferUsage::UniformBuffer,
		ERSMemoryUsage::HostVisible,
		sizeof(test),
		SRSResourceCreateInfo());

	/*Vertex = CGlobalShaderCompiler::Get().CompileShader(
		EShaderStage::Vertex,
		"Shaders/main.vert",
		"main",
		EShaderCompilerTarget::VulkanSpirV);

	Frag = CGlobalShaderCompiler::Get().CompileShader(
		EShaderStage::Fragment,
		"Shaders/main.frag",
		"main",
		EShaderCompilerTarget::VulkanSpirV);*/

	TestRenderPass = {
		/** Color attachments */
		{
			/** 0 */
			{
				Viewport->GetSurface()->GetSwapChainFormat(),
				ESampleCount::Sample1,
				ERSRenderPassAttachmentLoadOp::Clear,
				ERSRenderPassAttachmentStoreOp::Store,
				ERSRenderPassAttachmentLayout::Undefined,
				ERSRenderPassAttachmentLayout::Present
			}
		},

		/** Depth attachments */
		{
			/** 0 */
			{
				Viewport->GetDepthBuffer()->GetFormat(),
				ESampleCount::Sample1,
				ERSRenderPassAttachmentLoadOp::Clear,
				ERSRenderPassAttachmentStoreOp::Store,
				ERSRenderPassAttachmentLayout::Undefined,
				ERSRenderPassAttachmentLayout::DepthStencilAttachment
			}
		},

		/** Subpasses */
		{
			/** 0 */
			{
				/** Color attachment refs */
				{
					/** 0 */
					{
						0,
						ERSRenderPassAttachmentLayout::ColorAttachment
					}
				},

				/** Depth attachment refs */
				{
					{
						1,
						ERSRenderPassAttachmentLayout::DepthStencilAttachment
					}
				}
			}
		}
	};
}

glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightPos = glm::vec3(0, 5, 10);
glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 CameraDirection = glm::normalize(CameraPos - CameraTarget);
glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));
glm::vec3 CameraUp = glm::cross(CameraDirection, CameraRight);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
	float CamYaw = 0.f, CamPitch = 0.f;


void CEngineGame::Tick(SDL_Event* InEvent, const float& InDeltaTime)
{
	CEngine::Tick(InEvent, InDeltaTime);

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(
		currentTime - startTime).count();

	/** Update camera */
	const Uint8* KeyState = SDL_GetKeyboardState(nullptr);
	Uint32 MouseState = SDL_GetMouseState(nullptr, nullptr);
	static bool bIsMouseGrabbed = false;
	const float CameraSpeed = 0.0025f;

	if (MouseState & SDL_BUTTON_LMASK
		&& !bIsMouseGrabbed)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		bIsMouseGrabbed = true;
	}
	if (KeyState[SDL_SCANCODE_W])
	{
		CameraPos += CameraSpeed * CameraFront;
	}
	if (KeyState[SDL_SCANCODE_S])
	{
		CameraPos -= CameraSpeed * CameraFront;
	}
	if (KeyState[SDL_SCANCODE_A])
	{
		CameraPos -= glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
	}
	if (KeyState[SDL_SCANCODE_D])
	{
		CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed;
	}
	if (KeyState[SDL_SCANCODE_ESCAPE])
	{
		if (bIsMouseGrabbed)
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
			bIsMouseGrabbed = false;
		}
	}

	if (InEvent->type == SDL_MOUSEMOTION)
	{
		if (bIsMouseGrabbed)
		{
			float DeltaX = InEvent->motion.xrel * 0.25f;
			float DeltaY = InEvent->motion.yrel * 0.25f;

			CamYaw += DeltaX;
			CamPitch += -DeltaY;

			if (CamPitch > 89.0f)
				CamPitch = 89.0f;
			if (CamPitch < -89.0f)
				CamPitch = -89.0f;

			glm::vec3 Front;
			Front.x = cos(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
			Front.y = sin(glm::radians(CamPitch));
			Front.z = sin(glm::radians(CamYaw)) * cos(glm::radians(CamPitch));
			Front.y *= -1;
			CameraFront = glm::normalize(Front);

			SDL_WarpMouseInWindow((SDL_Window*) Window->GetHandle(),
				Window->GetWidth() / 2, Window->GetHeight() / 2);
		}
	}

	/** Draw the scene */
	EnqueueRenderCommand("CEngineGame::DrawWorld",
		[this, time]()
	{
		ZE::Renderer::SWorldRendererView View;
		View.Width = 1280;
		View.Height = 720;
		View.Surface = Viewport->GetSurface();
		ZE::Renderer::CClusteredForwardWorldRenderer Renderer;
		Viewport->Begin();

		/** Renderer */

		//if (Vertex._Is_ready())
		//{
		//	SShaderCompilerOutput& Output = Vertex.get();
		//	shaderV = GRenderSystem->CreateShader(
		//		EShaderStage::Vertex,
		//		Output.Bytecode.size(),
		//		Output.Bytecode.data(),
		//		Output.ReflectionData.ParameterMap);

		//	if (shaderV && shaderF) bshouldrendertri = true;
		//}

		//if (Frag._Is_ready())
		//{
		//	SShaderCompilerOutput& Output = Frag.get();
		//	shaderF = GRenderSystem->CreateShader(
		//		EShaderStage::Fragment,
		//		Output.Bytecode.size(),
		//		Output.Bytecode.data(),
		//		Output.ReflectionData.ParameterMap);

		//	if (shaderV && shaderF) bshouldrendertri = true;
		//}

		//if (bshouldrendertri)
		//{
		//	if (!pipeline)
		//	{
		//		pipeline = GRenderSystem->CreateGraphicsPipeline(
		//			{
		//				/** Vertex */
		//				{
		//					EShaderStage::Vertex,
		//					shaderV.get(),
		//					"main"
		//				},
		//				{
		//					EShaderStage::Fragment,
		//					shaderF.get(),
		//					"main"
		//				}
		//			},
		//			SStaticMeshVertex::GetBindingDescriptions(), 
		//			SStaticMeshVertex::GetAttributeDescriptions(), 
		//			TestRenderPass,
		//			SRSBlendState(),
		//			SRSRasterizerState(
		//				EPolygonMode::Fill,
		//				ECullMode::Back,
		//				EFrontFace::Clockwise),
		//			SRSDepthStencilState(
		//				true,
		//				true,
		//				ERSComparisonOp::GreaterOrEqual));
		//	}
		//}

		Renderer.Render(World->GetProxy(), View);
	
		//if(pipeline)
		//{
		//	//GRSContext->BindGraphicsPipeline(pipeline.get());
		//	//GRSContext->BindVertexBuffers({ testSM->GetRenderData()->GetVertexBuffer() });
		//	//GRSContext->BindIndexBuffer(testSM->GetRenderData()->GetIndexBuffer(),
		//	//	0, testSM->GetRenderData()->GetIndexFormat());
		//	//
		//	//		test WVP;
		//	//		glm::mat4 World = glm::scale(glm::mat4(1.0f), glm::vec3(1));
		//	//		glm::mat4 View = glm::lookAt(CameraPos, /*CameraPos
		//	//			+ */CameraFront,
		//	//			CameraUp);
		//	//		glm::mat4 Proj = glm::perspective(glm::radians(45.0f),
		//	//			1280 / (float) 720, 1000.f, 0.1f);
		//	//		Proj[1][1] *= -1;

		//	//		WVP.View = View;
		//	//		WVP.CameraPos = CameraPos;
		//	//		WVP.CameraFront = CameraFront;
		//	//		WVP.CameraUp = CameraUp;

		//	//		void* Dst = ubo->Map(ERSBufferMapMode::WriteOnly);
		//	//		memcpy(Dst, &WVP, sizeof(test));
		//	//		ubo->Unmap();

		//	//GRSContext->SetShaderUniformBuffer(0, 0, ubo.get());

		//	//GRSContext->DrawIndexed(testSM->GetIndexCount(), 1, 0, 0, 0);
		//}

		Viewport->End();
	});
}

CEngine* CreateEngine()
{
	return new CEngineGame;
}

} /* namespace ZE */