#include "Texture.h"

#include <DirectXTex.h>
#include "Data/FileIO.h"
#include "Utility/Logger.h"

namespace sl
{
	bool Texture::LoadAndCreateSRV(ID3D11Device* device, const std::wstring& filepath)
	{
		if (!device)
		{
			LOG_ERROR("Device‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ");
			return false;
		}
		if (!FileIO::FileExists_fs(filepath)) {
			LOG_ERROR("Texture::LoadAndCreateSRV - ƒtƒ@ƒCƒ‹‚ªŒ©‚Â‚©‚è‚Ü‚¹‚ñ: " + std::string(filepath.begin(), filepath.end()));
			return false;
		}

		DirectX::TexMetadata metadata;
		DirectX::ScratchImage image;

		HRESULT hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
		if (FAILED(hr))
		{
			LOG_ERROR("‰æ‘œ‚ğ“Ç‚İ‚ß‚Ü‚¹‚ñ‚Å‚µ‚½");
			return false;
		}

		hr = DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metadata, mShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("ShaderResourceView‚ğì¬‚Å‚«‚Ü‚¹‚ñ");
			return false;
		}

		LOG_INFO("TextureLoad‚Å‚«‚½ ");
		return true;
	}

	void Texture::CreateSamplarState(ID3D11Device* device)
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		if (FAILED(hr))
		{
			LOG_ERROR("SamplerState‚ğì¬‚Å‚«‚Ü‚¹‚ñ‚Å‚µ‚½");
		}
	}
}