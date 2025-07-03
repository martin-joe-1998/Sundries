#include "Shader.h"
#include <String>

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

HRESULT Shader::CreateTextureFromBMP(ID3D11Device1* device1, const wchar_t* fileName, ID3D11ShaderResourceView** textureSRV)
{
    HRESULT hr = S_OK;

    // Initialize WIC factory
    ComPtr<IWICImagingFactory> wicFactory;
    // hr = CoInitialize(nullptr);
    // if (FAILED(hr))
    // {
    //     MessageBox(nullptr, L"Failed to Initialize WICFactory.", L"Error", MB_OK | MB_ICONERROR);
    //     exit(0);
    // }

    hr = CoCreateInstance(
        CLSID_WICImagingFactory, 
        nullptr, 
        CLSCTX_INPROC_SERVER, 
        IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to Create WICFactory instance.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Decode Bitmap image
    ComPtr<IWICBitmapDecoder> decoder;
    hr = wicFactory->CreateDecoderFromFilename(
        fileName, 
        nullptr, 
        GENERIC_READ, 
        WICDecodeMetadataCacheOnLoad, 
        &decoder);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create decoder from filename.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to get frame.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Convert format to 32-bit RGBA(DXGI_FORMAT_R8G8B8A8_UNORM)
    ComPtr<IWICFormatConverter> converter;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create format converter.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    converter->Initialize(
        frame.Get(), 
        GUID_WICPixelFormat32bppRGBA, 
        WICBitmapDitherTypeNone, 
        nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to initialize converter.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    UINT width, height;
    converter->GetSize(&width, &height);

    std::vector<BYTE> pixelData(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, pixelData.size(), pixelData.data());

    // Create ID3D11Texture2D
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixelData.data();
    initData.SysMemPitch = width * 4;

    ComPtr<ID3D11Texture2D> texture;
    device1->CreateTexture2D(&texDesc, &initData, &texture);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create Texture2D.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    // Create Shader Resource View
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    device1->CreateShaderResourceView(texture.Get(), &srvDesc, textureSRV);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"Failed to create shader resource view.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }

    return S_OK;
}
