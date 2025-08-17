#pragma once
#include <iostream>
#include "Component.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Mesh.h"

#include "Utility/Math.h"

namespace sl
{
	class Actor;
	class Renderer;

	enum MeshType
	{
		ECircle, // 一旦はBaumkuchenのシェーダを使う
		EPizza,
		EBaumkuchen,
		ELine,
		EImage,
		EText
	};

	/// <summary>
	/// シェーダベースの描画コンポーネント
	/// </summary>
	class MeshComponent :	public Component
	{
	public:
		MeshComponent(Actor* owner, MeshType type);
		~MeshComponent();

		void Update(float deltaTime) override;
		void Draw(Renderer& renderer);

		void SetLineDir(Vector2 pos_) { mLineDir = pos_; }
		void SetSectorNum(int num_) { mSectorNum = num_; }
		void SetAngleOffset(float angle_) { mAngleOffset = angle_; }
		void SetRadius(float radius_) { mCurrentRadius = radius_; }
		void SetColor(Vector4 color_) { mColor = color_; }
		void SetChar(wchar_t text_);
		wchar_t GetChar() { return mChar; }
		void SetImageFileName(std::wstring value_);


		bool IsActive = true;
	private:

		std::unique_ptr<Shader> mShader = nullptr;
		std::unique_ptr<Texture> mTexture = nullptr;
		std::unique_ptr<ConstantBuffer> mCBuffer = nullptr;
		std::unique_ptr<Mesh> mMesh = nullptr;

		void CreateMesh(Renderer& renderer);
		void CreateShader(Renderer& renderer);
		void CreateTexture(Renderer& renderer);

		MeshType mMeshType = ECircle;		

		// Todo: MeshTypeによって使ってないパラメータあるので整備時に対応
		Vector2 mLineDir = Vector2(0.f, 0.f);
		int mSectorNum = 0;
		float mAngleOffset = 0.0f;
		float mCurrentRadius = 0.0f; // 45まで
		Vector4 mColor = Vector4(1.f, 1.f, 1.f, 1.f);
		wchar_t mChar = L'A';
		std::wstring mImageFileName = L"Title";

		void TfCBufferMapping(ID3D11DeviceContext* deviceContext);
		void ColorCBufferMapping(ID3D11DeviceContext* deviceContext, Vector4 color_);
		void PizzaParamCBufferMapping(ID3D11DeviceContext* deviceContext, int num_);
		void NoteParamCBufferMapping(ID3D11DeviceContext* deviceContext, float theta_);
		void NodeParamCBufferMapping(ID3D11DeviceContext* deviceContext, float angleOffset_, float radius_);
		void LineParamCBufferMapping(ID3D11DeviceContext* deviceContext, Vector2 dir_);

	};

}
