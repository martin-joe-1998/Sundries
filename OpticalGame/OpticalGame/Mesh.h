#pragma once
#include "Math.h"

class Mesh
{
public:
	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
		Vector4 color;	  // rgba
	};

	struct CBufferPermanent
	{
		Matrix4 viewMx;
		Matrix4 projectionMx;
	};

	struct CBufferEveryFrame
	{
		Matrix4 worldMx;
	};
};

