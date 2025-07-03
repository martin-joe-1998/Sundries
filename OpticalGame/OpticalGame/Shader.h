#pragma once
#include "Window.h"
#include "Renderer.h"
#include <wincodec.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

class Shader
{
public:
	static HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static const char* GetShaderModelForFeatureLevel(D3D_FEATURE_LEVEL level, const char* shaderTpe);
	static HRESULT CreateTextureFromBMP(
		ID3D11Device1* device1, 
		const wchar_t* fileName,
		ID3D11ShaderResourceView** textureSRV);
};

