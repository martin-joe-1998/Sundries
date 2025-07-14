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
    LOG_INFO("GraphicsDevice �̃��\�[�X��������܂���");
  }

  bool GraphicsDevice::CreateDevice(HWND& hwnd, int width, int height)
  {
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapChainDesc.BufferCount = 1; // �o�b�t�@��
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32�r�b�g�J���[
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �����_�[�^�[�Q�b�g�Ƃ��Ďg�p
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 4; // �A���`�G�C���A�V���O�T���v����
    swapChainDesc.Windowed = TRUE; // �E�B���h�E���[�hor �t���X�N���[��
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�ؑւ�����

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL, // �n�[�h�E�F�A�h���C�o�������I��
      D3D_DRIVER_TYPE_HARDWARE, // �n�[�h�E�F�A�h���C�o
      NULL, // �\�t�g�E�F�A�h���C�o�͎g�p���Ȃ�
      NULL, // �쐬�t���O�i�Ȃ��j
      NULL, // ����̋@�\���x���͎g�p���Ȃ�
      NULL, // �@�\���x���̐��i�Ȃ��j
      D3D11_SDK_VERSION,
      &swapChainDesc, // �X���b�v�`�F�C���̐ݒ� in
      mSwapChain.GetAddressOf(), // �X���b�v�`�F�C���̃|�C���^ out
      mDevice.GetAddressOf(), // Direct3D�f�o�C�X�̃|�C���^ out
      NULL, // �A�_�v�^�inullptr�Ńf�t�H���g�j out
      mDeviceContext.GetAddressOf() // �f�o�C�X�R���e�L�X�g�̃|�C���^ out
    );

    if (FAILED(hr))
    {
      LOG_ERROR("D3D11CreateDeviceAndSwapChain �Ɏ��s���܂��� (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    LOG_INFO("Direct3D �f�o�C�X�ƃX���b�v�`�F�C���̍쐬�ɐ������܂���");
    return true;
  }

  bool GraphicsDevice::CreateRenderTarget()
  {
    ID3D11Texture2D* backBuffer = nullptr;

    HRESULT hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr) || backBuffer == nullptr)
    {
      LOG_ERROR("�X���b�v�`�F�C������o�b�N�o�b�t�@�̎擾�Ɏ��s���܂��� (HRESULT = 0x" + std::to_string(hr) + ")");
      return false;
    }

    hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, mRenderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
      LOG_ERROR("�����_�[�^�[�Q�b�g�r���[�̍쐬�Ɏ��s���܂��� (HRESULT = 0x" + std::to_string(hr) + ")");
      backBuffer->Release();
      return false;
    }

    //backBuffer->GetDesc(&m_backBufferDesc); Todo
    backBuffer->Release();

    // Context �ɃZ�b�g
    mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
    LOG_INFO("�����_�[�^�[�Q�b�g�r���[�̍쐬�Ɛݒ�ɐ������܂���");
    return true;
  }
}
