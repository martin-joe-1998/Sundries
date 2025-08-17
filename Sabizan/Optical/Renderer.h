#pragma once
#include "../Window.h"
#include <vector>
#include <memory>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Game;
class SpriteComponent;
class CBuffer;

class Renderer
{
public:
	Renderer(Game* mGame);
	~Renderer();
	bool Initialize(int screenWidth, int screenHeight);
	void BeginFrame();
	void EndFrame();
	void Draw();

	void ClearSprites();

	void AddSprite(SpriteComponent* sprite);
	//void RemoveSprite(class SpriteComponent* sprite);

	ID3D11Device1* GetDevice() const { return m_device1; }
	ID3D11DeviceContext1* GetDeviceContext() const { return m_deviceContext1; }

	Game* GetGame() const { return mGame; }
	Window* GetWindow() const { return mWindow; }
	int GetWidth() const { return mWindowWidth; }
	int GetHeight() const { return mWindowHeight; }
	D3D_FEATURE_LEVEL GetFeatureLevel() const { return mSelectedFeatureLevel; }
	ID3D11BlendState* GetBlendState() const { return m_blendState; }

	std::shared_ptr<CBuffer> GetCBufferRef() const { return m_CBuffer; }
	float GetBackBufferWidth() { return m_backBufferDesc.Width; }
	float GetBackBufferHeight() { return m_backBufferDesc.Height; }
private:
	void CreateDevice(Window& window);
	void CreateRenderTarget();
	void CreateDepthStencil();
	void CreateBlendState();
	void CreateCBuffer();

	void CreatePostProcessResources();

	// Device stuff
	IDXGISwapChain* m_swapChain;
	//ID3D11Device* m_device = nullptr;
	//ID3D11DeviceContext* m_deviceContext = nullptr;

	ID3D11Device1* m_device1;
	ID3D11DeviceContext1* m_deviceContext1;

	// Render target view
	ID3D11RenderTargetView* m_renderTargetView;
	D3D11_TEXTURE2D_DESC m_backBufferDesc{};

	// Depth stencil
	ID3D11Texture2D* m_depthStencil;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11DepthStencilState* m_depthStencilState;

	// Blend State
	ID3D11BlendState* m_blendState = nullptr;

	std::shared_ptr<CBuffer> m_CBuffer = nullptr;

	int mWindowWidth{};
	int mWindowHeight{};

	Game* mGame;
	Window* mWindow;

	D3D_FEATURE_LEVEL mSelectedFeatureLevel;

	// All the sprite components drawn
	std::vector<SpriteComponent*> mSprites;

	// Post processing
	// Post-processing Render Target
	ID3D11Texture2D* m_postProcessTexture = nullptr;
	ID3D11RenderTargetView* m_postProcessRTV = nullptr;
	ID3D11ShaderResourceView* m_postProcessSRV = nullptr;

	// Fullscreen quad for post-processing
	ID3D11Buffer* m_fullscreenQuadVB = nullptr;
	ID3D11Buffer* m_fullscreenQuadIB = nullptr;

	// Post-processing shaders
	ID3D11VertexShader* m_postProcessVS = nullptr;
	ID3D11PixelShader* m_postProcessPS = nullptr;
	ID3D11InputLayout* m_postProcessInputLayout = nullptr;

	ID3D11SamplerState* m_postProcessSampler = nullptr;
};