#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

#include <filesystem>


using Microsoft::WRL::ComPtr;

namespace sl
{
	/// <summary>
	/// 僔僃乕僟乕偺儘乕僪丄僐儞僷僀儖丄揔梡傪娗棟
	/// </summary>
	class Shader
	{
	public:
		bool Load(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& filename, const std::string& vsEntry, const std::string& psEntry);
		ComPtr<ID3DBlob> CompileShader(const std::wstring& path, const std::string& entryPoint, const std::string& target);

		bool CreateShaders(ID3D11Device* device, ID3DBlob* vsBlob, ID3DBlob* psBlob);
		bool CreateLayout(ID3D11Device* device, ID3DBlob* vsBlob);
		void Apply(ID3D11DeviceContext* context);

		ID3D11VertexShader* GetVertexShader() { return mVertexShader.Get(); }
		ID3D11PixelShader* GetPixelShader() { return mPixelShader.Get(); }
		ID3D11InputLayout* GetInputLayout() { return mInputLayout.Get(); }

	private:
		ComPtr<ID3D11VertexShader> mVertexShader;
		ComPtr<ID3D11PixelShader> mPixelShader;
		ComPtr<ID3D11InputLayout> mInputLayout;




		// Todo: FileIO偲偐偵堏怉偟傛偆
		static bool FileExists_fs(const std::wstring& path)
		{
			return std::filesystem::exists(path);
		}
	};
}

