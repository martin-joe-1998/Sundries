#pragma once
#include <memory>
#include <string>
#include "Component.h"
#include "../Math.h"
#include "../Transform.h"
#include "../Graphics/Texture.h"
#include "../Graphics/CBuffer.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Mesh.h"

class SpriteComponent : public Component
{
public:
	// (Lower draw order corresponds with further back)
	SpriteComponent(class Actor* owner, const std::vector<std::string>& textureFileNames, const std::string& shaderFileName, int drawOrder = 100);
	virtual ~SpriteComponent();

	virtual void Draw(class Renderer& renderer);
	void Update(float deltaTime) override {}
	void OnUpdateWorldTransform() override {}

	int GetDrawOrder() const { return mDrawOrder; }
	int GetTexHeight() const { return mTexHeight; }
	int GetTexWidth() const { return mTexWidth; }

	void SetVisible(bool visible) { mVisible = visible; }
	bool GetVisible() const { return mVisible; }

private:
	int mDrawOrder;
	int mTexWidth;
	int mTexHeight;
	bool mVisible;

	void CreateMesh(class Renderer& renderer);
	void CreateShaders(class Renderer& renderer);
	void SetCBuffer(ID3D11DeviceContext1* deviceContext);

	std::vector<std::unique_ptr<Texture>> m_textures;
	// Renderer が持っている唯一の CBuffer を参照するためのポインター
	std::shared_ptr<CBuffer> mCbufferRef = nullptr;
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Mesh> mMesh = nullptr;

	std::string mShaderName;
};

