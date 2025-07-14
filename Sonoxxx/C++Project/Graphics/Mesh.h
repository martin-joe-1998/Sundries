#pragma once
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include "Vertex.h"

namespace sl
{
	class Renderer;

	class Mesh
	{
	public:
		void CreateMesh(Renderer& renderer);

		const std::vector<Vertex>& GetVertices() const { return mVertices; }
		ID3D11Buffer* GetVertexBuffer() const { return mVertexBuffer.Get(); }

	private:
		std::vector<Vertex> mVertices;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

		void SetVertices(const std::vector<Vertex>& vertices) { mVertices = vertices; }
		void CreateVertexBuffer(ID3D11Device* device);

	};
}
