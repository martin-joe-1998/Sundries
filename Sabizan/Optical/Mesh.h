#pragma once
#include "../Math.h"
#include "Renderer.h"

class Mesh
{
public:
	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
		Vector4 color;	  // rgba
	};

	struct FullscreenQuadVertex
	{
		float position[3]; // x, y, z
		float texcoord[2]; // u, v
	};

	void CreateMesh(Renderer& renderer);
	void SetMeshBuffer(Renderer& renderer);
	void CreateFullscreenVertex(Renderer& renderer);

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
};

