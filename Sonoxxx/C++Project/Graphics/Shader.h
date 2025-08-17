#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

#include "Utility/Math.h"


using Microsoft::WRL::ComPtr;

namespace sl
{
	/// <summary>
	/// シェーダーのロード、コンパイル、適用を管理
	/// </summary>
	class Shader
	{
	public:
		bool Load(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filename, const std::string& vsEntry, const std::string& psEntry);


		ID3D11VertexShader* GetVertexShader() { return mVertexShader.Get(); }
		ID3D11PixelShader* GetPixelShader() { return mPixelShader.Get(); }
		ID3D11InputLayout* GetInputLayout() { return mInputLayout.Get(); }

	private:
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;
		ComPtr<ID3D11InputLayout> mInputLayout;

		ComPtr<ID3DBlob> CompileShader(const std::wstring& path, const std::string& entryPoint, const std::string& target);
		bool CreateShaders(ID3D11Device* device, ID3DBlob* vsBlob, ID3DBlob* psBlob);
		bool CreateLayout(ID3D11Device* device, ID3DBlob* vsBlob);
	};
}

