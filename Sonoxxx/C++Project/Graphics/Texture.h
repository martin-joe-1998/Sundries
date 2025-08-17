#pragma once

#include <string>
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace sl
{
	class Texture
	{
	public:
		bool LoadAndCreateSRV(ID3D11Device* device, const std::wstring& filepath);
		void CreateSamplarState(ID3D11Device* device);

		ID3D11ShaderResourceView* const* GetShaderResourceView() { return mShaderResourceView.GetAddressOf(); }
		ID3D11SamplerState* const* GetSamperState() { return mSamplerState.GetAddressOf(); }
	private:
		ComPtr<ID3D11ShaderResourceView> mShaderResourceView = nullptr;
		ComPtr<ID3D11SamplerState> mSamplerState = nullptr;
	};
}