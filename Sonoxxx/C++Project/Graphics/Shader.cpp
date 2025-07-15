#include "Shader.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "Utility/Logger.h"

namespace sl
{
  bool Shader::Load(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& filepath, const std::string& vsEntry, const std::string& psEntry) {
    if (!device)
    {
      LOG_ERROR("Device偑尒偮偐傝傑偣傫");
    }
    
    if (!FileExists_fs(filepath)) {
      LOG_ERROR("Shader::Load - 僼傽僀儖偑尒偮偐傝傑偣傫: " + std::string(filepath.begin(), filepath.end()));
      return false;
    }

    ComPtr<ID3DBlob> vsBlob = CompileShader(filepath, vsEntry, "vs_5_0");
    ComPtr<ID3DBlob> psBlob = CompileShader(filepath, psEntry, "ps_5_0");
    if (!vsBlob || !psBlob) {
      LOG_ERROR("Shader::Load - 僔僃乕僟偺僐儞僷僀儖偵幐攕偟傑偟偨");
      return false;
    }

    if (!CreateShaders(device, vsBlob.Get(), psBlob.Get())) {
      LOG_ERROR("Shader::Load - 僔僃乕僟僆僽僕僃僋僩偺嶌惉偵幐攕偟傑偟偨");
      return false;
    }
    if (!CreateLayout(device, vsBlob.Get())) {
      LOG_ERROR("Shader::Load - 擖椡儗僀傾僂僩偺嶌惉偵幐攕偟傑偟偨");
      return false;
    }

    Apply(context);

    LOG_INFO("Shader::Load - 僔僃乕僟偺撉傒崬傒偵惉岟偟傑偟偨");
    return true;
  }

  void Shader::Apply(ID3D11DeviceContext* context) {
      // Shader のセットは SpriteComponent::Draw でやっているので、ここの二行を消した
      // InputLayout のセットは今みたいに、CreateLayout の直後にやるので大丈夫だと思う！
      context->IASetInputLayout(mInputLayout.Get());
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
        LOG_ERROR("Shader::CompileShader - 僐儞僷僀儖僄儔乕:\n" + errorMsg);
      }

      LOG_ERROR("Shader::CompileShader - 僐儞僷僀儖偵幐攕偟傑偟偨: " + entryPoint + " (" + target + ")");
      return nullptr;
    }

    LOG_INFO("Shader::CompileShader - 僐儞僷僀儖惉岟: " + entryPoint + " (" + target + ")");
    return shaderBlob;
  }

  bool Shader::CreateShaders(ID3D11Device* device, ID3DBlob* vsBlob, ID3DBlob* psBlob)
  {
    HRESULT hrVS = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, mVertexShader.GetAddressOf());
    HRESULT hrPS = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, mPixelShader.GetAddressOf());

    if (FAILED(hrVS)) LOG_ERROR("Shader::CreateShaders - 捀揰僔僃乕僟偺嶌惉偵幐攕偟傑偟偨");
    if (FAILED(hrPS)) LOG_ERROR("Shader::CreateShaders - 僺僋僙儖僔僃乕僟偺嶌惉偵幐攕偟傑偟偨");

    return SUCCEEDED(hrVS) && SUCCEEDED(hrPS);
  }

  bool Shader::CreateLayout(ID3D11Device* device, ID3DBlob* vsBlob)
  {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    HRESULT hr = device->CreateInputLayout(layout, ARRAYSIZE(layout),
      vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
      mInputLayout.GetAddressOf());

    if (FAILED(hr)) {
      LOG_ERROR("Shader::CreateLayout - 擖椡儗僀傾僂僩偺嶌惉偵幐攕偟傑偟偨");
      return false;
    }

    return true;
  }

}