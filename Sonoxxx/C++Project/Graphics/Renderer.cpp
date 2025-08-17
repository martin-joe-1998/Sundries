#include "Renderer.h"
#include "Game/Game.h"
#include "Scenes/SceneBase.h"
#include "Game/Actor.h"

#include "Utility/Logger.h"
#include "Data/Config.h"
#include "Application/Application.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/MeshComponent.h"


namespace sl
{
  Renderer::Renderer(Game* game)
    : mGame(game)
  {
      // Post processing
      mPostProcess = std::make_unique<PostProcessing>(this);
      // Post processing のOn/Offを制御する変数
      mIsActivePostProcess = TRUE;
  }

  Renderer::~Renderer()
  {
    if (mSwapChain) mSwapChain->SetFullscreenState(FALSE, nullptr);
    mRenderTargetView = nullptr;
    mSwapChain = nullptr;
    mDeviceContext = nullptr;
    mDevice = nullptr;
    //LOG_INFO("GraphicsDevice 偺儕僜乕僗傪夝曻偟傑偟偨");

    // Blend State
    mBlendState = nullptr;

    // Post processing
    mPostProcess = nullptr;
  }

  void Renderer::Initialize(/*HWND hwnd, int width, int height*/) {
    HWND hwnd = Application::GetInstance().GetWindowHandle();
	mWindowWidth = Application::GetInstance().GetWindowWidth();
    mWindowHeight = Application::GetInstance().GetWindowHeight();
    
    // 僨僶僀僗偺弶婜壔
    CreateDevice(hwnd, mWindowWidth, mWindowHeight);
    CreateRenderTarget();
    // Blend State
    CreateBlendState();

    // Post processing
    mPostProcess->CreatePostProcessResources();
  }

  bool Renderer::CreateDevice(HWND& hwnd, int width, int height)
  {
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount = 1; // 僶僢僼傽悢
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // 8 -> 16, 32はダメ、CreateDevice で失敗する
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 儗儞僟乕僞乕僎僢僩偲偟偰巊梡
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 4; // 傾儞僠僄僀儕傾僔儞僌僒儞僾儖悢
    swapChainDesc.Windowed = TRUE; // 僂傿儞僪僂儌乕僪or 僼儖僗僋儕乕儞
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 儌乕僪愗懼傪嫋壜

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL, // 僴乕僪僂僃傾僪儔僀僶傪帺摦慖戰
      D3D_DRIVER_TYPE_HARDWARE, // 僴乕僪僂僃傾僪儔僀僶
      NULL, // 僜僼僩僂僃傾僪儔僀僶偼巊梡偟側偄
      NULL, // 嶌惉僼儔僌乮側偟乯
      NULL, // 摿掕偺婡擻儗儀儖偼巊梡偟側偄
      NULL, // 婡擻儗儀儖偺悢乮側偟乯
      D3D11_SDK_VERSION,
      &swapChainDesc, // 僗儚僢僾僠僃僀儞偺愝掕 in
      mSwapChain.GetAddressOf(), // 僗儚僢僾僠僃僀儞偺億僀儞僞 out
      mDevice.GetAddressOf(), // Direct3D僨僶僀僗偺億僀儞僞 out
      NULL, // 傾僟僾僞乮nullptr偱僨僼僅儖僩乯 out
      mDeviceContext.GetAddressOf() // 僨僶僀僗僐儞僥僉僗僩偺億僀儞僞 out
    );

    if (FAILED(hr))
    {
      LOG_ERROR("D3D11CreateDeviceAndSwapChain 偵幐攕偟傑偟偨 (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    //LOG_INFO("Direct3D 僨僶僀僗偲僗儚僢僾僠僃僀儞偺嶌惉偵惉岟偟傑偟偨");
    return true;
  }

  bool Renderer::CreateRenderTarget()
  {
    ID3D11Texture2D* backBuffer = nullptr;

    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr) || backBuffer == nullptr)
    {
      LOG_ERROR("僗儚僢僾僠僃僀儞偐傜僶僢僋僶僢僼傽偺庢摼偵幐攕偟傑偟偨 (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, mRenderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
      LOG_ERROR("儗儞僟乕僞乕僎僢僩價儏乕偺嶌惉偵幐攕偟傑偟偨 (HRESULT = 0x" + std::to_string(hr) + ")");
      backBuffer->Release();
      return false;
    }

    backBuffer->GetDesc(&mBackBufferDesc);
    backBuffer->Release();

    // Context 偵僙僢僩
    mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
    //LOG_INFO("儗儞僟乕僞乕僎僢僩價儏乕偺嶌惉偲愝掕偵惉岟偟傑偟偨");
    return true;
  }

  // Blend State は一旦作ったら、属性を変更することはできない
  // ここでは、半透明ブレンド用の Blend State を作成している
  bool Renderer::CreateBlendState()
  {
      D3D11_BLEND_DESC blendDesc = {};
      blendDesc.RenderTarget[0].BlendEnable = TRUE;
      blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      HRESULT hr = mDevice->CreateBlendState(&blendDesc, &mBlendState);
      if (FAILED(hr))
      {
          LOG_ERROR("Failed to create blend state trans (HRESULT = 0x" + std::to_string(hr) + ")");
          return false;
      }

      return true;
  }

  void Renderer::BeginFrame()
  {
    auto context = mDeviceContext.Get();

    if (mIsActivePostProcess)
    {
        auto postProcessRTV = mPostProcess->GetRTV();
        context->OMSetRenderTargets(1, &postProcessRTV, NULL);
        context->ClearRenderTargetView(postProcessRTV, Config::Common::BackGroundColor);
    }
    else
    {
        context->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), NULL);
        context->ClearRenderTargetView(mRenderTargetView.Get(), Config::Common::BackGroundColor);
    }

    // Set viewport
    D3D11_VIEWPORT viewport = {};
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0.0f; // 價儏乕億乕僩偺嵍忋X嵗昗
    viewport.TopLeftY = 0.0f; // 價儏乕億乕僩偺嵍忋Y嵗昗
    viewport.Width = static_cast<FLOAT>(mWindowWidth); // 價儏乕億乕僩偺暆
    viewport.Height = static_cast<FLOAT>(mWindowHeight); // 價儏乕億乕僩偺崅偝
    context->RSSetViewports(1, &viewport); // 價儏乕億乕僩傪愝掕
  }

  void Renderer::Draw()
  {
      auto& currentScene = mGame->GetScene();
      
      if (currentScene)
      {
          const auto& actors = currentScene->GetActors();
          for (const auto& actor : actors)
          {
              // Todo: Transform偺僙僢僩
              //if (auto spriteComp = actor->GetComponent<SpriteComponent>())
              //{
              //	spriteComp->Draw(*mRenderer);
              //}
  
              if (auto meshComp = actor->GetComponent<MeshComponent>())
              {
                  if (meshComp->IsActive)
                  {
                      meshComp->Draw(*this);
                  }
              }
          }
      }
      else
      {
          LOG_WARN("CurrentScene is null");
      }
  }

  void Renderer::EndFrame()
  {
    mSwapChain->Present(1, 0);
  }

  void Renderer::PostProcessPass()
  {
      if (mIsActivePostProcess)
      {
        mPostProcess->RunPass();
      }
  }
}
