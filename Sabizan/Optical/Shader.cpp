#include "Shader.h"

Shader::Shader(const std::string& shaderName) :
    m_shaderName(shaderName)
{
    auto shaderPath = m_shaderFileFolder + m_shaderName;
    wideShaderFileName = std::wstring(shaderPath.begin(), shaderPath.end());
    m_lpShaderFilePath = wideShaderFileName.c_str();
}

Shader::~Shader()
{
    Unload();
}

void Shader::Unload()
{
    if (m_vertexShader) m_vertexShader->Release(); m_vertexShader = nullptr;
    if (m_pixelShader) m_pixelShader->Release(); m_pixelShader = nullptr;
    if (m_inputLayout) m_inputLayout->Release(); m_inputLayout = nullptr;
}

HRESULT Shader::CreatShaderAndInputLayout(Renderer& renderer, D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutSize)
{
    HRESULT hr;

    // ----------------------------- Vertex Shader -----------------------------
    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    D3D_FEATURE_LEVEL featureLevel = renderer.GetFeatureLevel();
    const char* vshaderModel = Shader::GetShaderModelForFeatureLevel(featureLevel, "vs");

    hr = Shader::CompileShaderFromFile(
        m_lpShaderFilePath,
        "VSMain", vshaderModel, &pVSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to compile vertex shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Create the vertex shader
    hr = renderer.GetDevice()->CreateVertexShader(
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        nullptr,
        &m_vertexShader
    );
    if (FAILED(hr))
    {
        pVSBlob->Release();
        MessageBox(nullptr, L"Failed to create vertex shader.", L"Error", MB_OK);
        exit(0);
    }

    // ----------------------------- Input Layout -----------------------------
    // Define the input layout
    // { LPCSTR SemanticName, 
    //	 UINT SemanticIndex,	(TEXCOORD0, TEXCOORD1...)
    //	 DXGI_FORMAT Format, 
    //	 UINT InputSlot,		(Which vertex buffer?)
    //	 UINT AlignedByteOffset, (D3D11_APPEND_ALIGNED_ELEMENT will do anything for me, woohoo)
    //	 D3D11_INPUT_CLASSIFICATION InputSlotClass, 
    //	 UINT InstanceDataStepRate (Only valid when using instancing) }

    // Create the input layout
    hr = renderer.GetDevice()->CreateInputLayout(
        layout,
        layoutSize,
        pVSBlob->GetBufferPointer(),
        pVSBlob->GetBufferSize(),
        &m_inputLayout
    );
    // Release the vertex shader blob as it's no longer needed
    pVSBlob->Release();

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create input layout.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Set the input layout
    renderer.GetDeviceContext()->IASetInputLayout(m_inputLayout);


    // ----------------------------- Pixel Shader -----------------------------
    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    const char* pshaderModel = Shader::GetShaderModelForFeatureLevel(featureLevel, "ps");

    hr = Shader::CompileShaderFromFile(
        m_lpShaderFilePath,//L"Shaders/RectangleShader.fxh",
        "PSMain", pshaderModel, &pPSBlob);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to compile pixel shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Create the pixel shader
    hr = renderer.GetDevice()->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(),
        nullptr,
        &m_pixelShader
    );
    pPSBlob->Release();
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create pixel shader.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    return S_OK;
}

HRESULT Shader::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
        }
        return hr;
    }
    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

const char* Shader::GetShaderModelForFeatureLevel(D3D_FEATURE_LEVEL level, const char* shaderType)
{
    static thread_local std::string result;
    std::string model;

    switch (level)
    {
    case D3D_FEATURE_LEVEL_12_1:
    case D3D_FEATURE_LEVEL_12_0:
        model = "_5_1"; break;
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
        model = "_5_0"; break;
    case D3D_FEATURE_LEVEL_10_1:
        model = "_4_1"; break;
    case D3D_FEATURE_LEVEL_10_0:
        model = "_4_0"; break;
    case D3D_FEATURE_LEVEL_9_3:
        model = "_4_0_level_9_3"; break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
        model = "_4_0_level_9_1"; break;
    default:
        model = "_5_0"; break;
    }

    result = std::string(shaderType) + model;
    return result.c_str();
}

