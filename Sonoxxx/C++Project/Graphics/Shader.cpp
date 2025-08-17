#include "Shader.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "Data/FileIO.h"
#include "Utility/Logger.h"

namespace sl
{
  bool Shader::Load(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filepath, const std::string& vsEntry, const std::string& psEntry) {
    if (!device)
    {
      LOG_ERROR("Deviceが見つかりません");
    }
    
    if (!FileIO::FileExists_fs(filepath)) {
      LOG_ERROR("Shader::Load - ファイルが見つかりません: " + std::string(filepath.begin(), filepath.end()));
      return false;
    }

    ComPtr<ID3DBlob> vsBlob = CompileShader(filepath, vsEntry, "vs_5_0");
    ComPtr<ID3DBlob> psBlob = CompileShader(filepath, psEntry, "ps_5_0");
    if (!vsBlob || !psBlob) {
      LOG_ERROR("Shader::Load - シェーダのコンパイルに失敗しました");
      return false;
    }

    if (!CreateShaders(device, vsBlob.Get(), psBlob.Get())) {
      LOG_ERROR("Shader::Load - シェーダオブジェクトの作成に失敗しました");
      return false;
    }
    if (!CreateLayout(device, vsBlob.Get())) {
      LOG_ERROR("Shader::Load - 入力レイアウトの作成に失敗しました");
      return false;
    }

    deviceContext->IASetInputLayout(mInputLayout.Get());
    //context->VSSetShader(mVertexShader.Get(), nullptr, 0); // SpriteComponentでやってる
    //context->PSSetShader(mPixelShader.Get(), nullptr, 0);

    //LOG_INFO("Shader::Load - シェーダの読み込みに成功しました");
    return true;
  }

  ComPtr<ID3DBlob> Shader::CompileShader(const std::wstring& path, const std::string& entryPoint, const std::string& target)
  {
    ComPtr<ID3DBlob> shaderBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr,
      entryPoint.c_str(), target.c_str(),
      0, 0, &shaderBlob, &errorBlob);

    if (FAILED(hr)) {
      if (errorBlob) {
        std::string errorMsg((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
        LOG_ERROR("Shader::CompileShader - コンパイルエラー:\n" + errorMsg);
      }

      LOG_ERROR("Shader::CompileShader - コンパイルに失敗しました: " + entryPoint + " (" + target + ")");
      return nullptr;
    }

    //LOG_INFO("Shader::CompileShader - コンパイル成功: " + entryPoint + " (" + target + ")");
    return shaderBlob;
  }

  bool Shader::CreateShaders(ID3D11Device* device, ID3DBlob* vsBlob, ID3DBlob* psBlob)
  {
    HRESULT hrVS = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, mVertexShader.GetAddressOf());
    HRESULT hrPS = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, mPixelShader.GetAddressOf());

    if (FAILED(hrVS)) LOG_ERROR("Shader::CreateShaders - 頂点シェーダの作成に失敗しました");
    if (FAILED(hrPS)) LOG_ERROR("Shader::CreateShaders - ピクセルシェーダの作成に失敗しました");

    return SUCCEEDED(hrVS) && SUCCEEDED(hrPS);
  }

  bool Shader::CreateLayout(ID3D11Device* device, ID3DBlob* vsBlob)
  {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,       D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(layout, ARRAYSIZE(layout),
      vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
      mInputLayout.GetAddressOf());

    if (FAILED(hr)) {
      LOG_ERROR("Shader::CreateLayout - 入力レイアウトの作成に失敗しました");
      return false;
    }

    return true;
  }

}