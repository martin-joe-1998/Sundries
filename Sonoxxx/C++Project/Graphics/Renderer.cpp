#include "Renderer.h"

#include "Utility/Logger.h"
#include "Application/Application.h"
#include "Game/Components/SpriteComponent.h"


namespace sl
{
    Renderer::Renderer(Game* game)
      : mGame(game)
    {
    
    }
    
    Renderer::~Renderer()
    {
        if (mSwapChain) mSwapChain->SetFullscreenState(FALSE, nullptr);
        mRenderTargetView = nullptr;
        mSwapChain = nullptr;
        mDeviceContext = nullptr;
        mDevice = nullptr;
        LOG_INFO("GraphicsDevice 偺儕僜乕僗傪夝曻偟傑偟偨");
    }
    
    void Renderer::Initialize(/*HWND hwnd, int width, int height*/) {
        HWND hwnd = Application::GetInstance().GetWindowHandle();
     	mWindowWidth = Application::GetInstance().GetWindowWidth();
        mWindowHeight = Application::GetInstance().GetWindowHeight();
        
        // 僨僶僀僗偺弶婜壔
        CreateDevice(hwnd, mWindowWidth, mWindowHeight);
        CreateRenderTarget();
    }
    
    void Renderer::Draw()
    {
        BeginFrame();
    
        for (auto sprite : mSpriteComponents)
        {
          //if (sprite->GetVisible()) // NowTodo: sprite僋儔僗偺愝掕
          //{
            sprite->Draw(*this);
          //}
        }
    
        EndFrame();
    }
    
    void Renderer::AddSprite(SpriteComponent* sc)
    {
        mSpriteComponents.emplace_back(sc);
    }
    
    void Renderer::RemoveSprite(SpriteComponent* sc)
    {
        auto iter = std::find(mSpriteComponents.begin(), mSpriteComponents.end(), sc);
        mSpriteComponents.erase(iter);
    }
    
    bool Renderer::CreateDevice(HWND& hwnd, int width, int height)
    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        swapChainDesc.BufferCount = 1; // 僶僢僼傽悢
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32價僢僩僇儔乕
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
    
        LOG_INFO("Direct3D 僨僶僀僗偲僗儚僢僾僠僃僀儞偺嶌惉偵惉岟偟傑偟偨");
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
    
        //backBuffer->GetDesc(&m_backBufferDesc); Todo
        backBuffer->Release();
    
        // Context 偵僙僢僩
        mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
        LOG_INFO("儗儞僟乕僞乕僎僢僩價儏乕偺嶌惉偲愝掕偵惉岟偟傑偟偨");
        return true;
    }
    
    void Renderer::BeginFrame()
    {
        auto context = mDeviceContext.Get();
        context->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), NULL);
    
        D3D11_VIEWPORT viewport = {};
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
        viewport.TopLeftX = 0.0f; // 價儏乕億乕僩偺嵍忋X嵗昗
        viewport.TopLeftY = 0.0f; // 價儏乕億乕僩偺嵍忋Y嵗昗
        viewport.Width = static_cast<FLOAT>(mWindowWidth); // 價儏乕億乕僩偺暆
        viewport.Height = static_cast<FLOAT>(mWindowHeight); // 價儏乕億乕僩偺崅偝
        context->RSSetViewports(1, &viewport); // 價儏乕億乕僩傪愝掕
    
        float clearColor[] = { 0.f, 1.f, 0.f, 1.f }; // initColor
        context->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
    }
    
    
    void Renderer::EndFrame()
    {
        mSwapChain->Present(1, 0);
    }
}
