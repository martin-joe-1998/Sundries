#include "SpriteComponent.h"
#include "Game/Actor.h"
#include "Game/Game.h"
#include "Graphics/Renderer.h"

namespace sl
{
	SpriteComponent::SpriteComponent(Actor* owner)
		: Component(owner)
	{
		mShader = std::make_unique<Shader>();

		auto renderer = mOwner->GetGame()->GetRenderer();
		CreateMesh(*renderer);
		CreateShader(*renderer);
		renderer->AddSprite(this); // Renderer偵搊榐
	}

	SpriteComponent::~SpriteComponent()
	{
		mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
	}

	void SpriteComponent::Draw(Renderer& renderer)
	{
		
		auto deviceContext = renderer.GetDeviceContext();
		// 掕悢僶僢僼傽偺愝掕側偳

		// Set the rectangle shader
		deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
		//deviceContext->VSSetConstantBuffers(0, 1, &_cbPermanent);
		//deviceContext->VSSetConstantBuffers(1, 1, &_cbPerFrame);
		deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);

		deviceContext->DrawIndexed(6, 0, 0);

	}

	void SpriteComponent::CreateMesh(Renderer& renderer)
	{
		Mesh* mesh = new Mesh();
		mesh->CreateMesh(renderer);
		delete mesh;


	}
	void SpriteComponent::CreateShader(Renderer& renderer)
	{
		mShader->Load(renderer.GetDevice(), renderer.GetDeviceContext(), L"Assets/Shaders/Test.shader", "VShader", "PShader");
	}
}
