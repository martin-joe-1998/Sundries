#include "GraphicsDevice.h"
#include "Utility/Logger.h"

namespace sl
{
	void GraphicsDevice::Initialize(HWND hwnd, int width, int height)
	{
    CreateDevice(hwnd, width, height);
    CreateRenderTarget();
  }

  void GraphicsDevice::Cleanup() {
    if (mSwapChain) mSwapChain->SetFullscreenState(FALSE, nullptr);
    mRenderTargetView = nullptr;
    mSwapChain = nullptr;
    mDeviceContext = nullptr;
    mDevice = nullptr;
    LOG_INFO("GraphicsDevice のリソースを解放しました");
  }

  bool GraphicsDevice::CreateDevice(HWND& hwnd, int width, int height)
  {
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount = 1; // バッファ数
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32ビットカラー
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // レンダーターゲットとして使用
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 4; // アンチエイリアシングサンプル数
    swapChainDesc.Windowed = TRUE; // ウィンドウモードor フルスクリーン
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // モード切替を許可

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL, // ハードウェアドライバを自動選択
      D3D_DRIVER_TYPE_HARDWARE, // ハードウェアドライバ
      NULL, // ソフトウェアドライバは使用しない
      NULL, // 作成フラグ（なし）
      NULL, // 特定の機能レベルは使用しない
      NULL, // 機能レベルの数（なし）
      D3D11_SDK_VERSION,
      &swapChainDesc, // スワップチェインの設定 in
      mSwapChain.GetAddressOf(), // スワップチェインのポインタ out
      mDevice.GetAddressOf(), // Direct3Dデバイスのポインタ out
      NULL, // アダプタ（nullptrでデフォルト） out
      mDeviceContext.GetAddressOf() // デバイスコンテキストのポインタ out
    );

    if (FAILED(hr))
    {
      LOG_ERROR("D3D11CreateDeviceAndSwapChain に失敗しました (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    LOG_INFO("Direct3D デバイスとスワップチェインの作成に成功しました");
    return true;
  }

  bool GraphicsDevice::CreateRenderTarget()
  {
    ID3D11Texture2D* backBuffer = nullptr;

    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr) || backBuffer == nullptr)
    {
      LOG_ERROR("スワップチェインからバックバッファの取得に失敗しました (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, mRenderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
      LOG_ERROR("レンダーターゲットビューの作成に失敗しました (HRESULT = 0x" + std::to_string(hr) + ")");
      backBuffer->Release();
      return false;
    }

    //backBuffer->GetDesc(&m_backBufferDesc); Todo
    backBuffer->Release();

    // Context にセット
    mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
    LOG_INFO("レンダーターゲットビューの作成と設定に成功しました");
    return true;
  }
}
