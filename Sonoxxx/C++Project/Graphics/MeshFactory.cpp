#include "MeshFactory.h"
#include <d3d11.h>
#include <d3dcommon.h>

#include <vector>
#include <Utility/Math.h>

namespace sl
{
  //std::shared_ptr<Mesh> MeshFactory::CreateQuad(ID3D11Device* device, float width, float height, const Vector4& color)
  //{
  //  auto mesh = std::make_shared<Mesh>();
  //  std::vector<Vertex> vertices(6);

  //  float hw = width * 0.5f;
  //  float hh = height * 0.5f;

  //  // 2ŽOŠpŒ`‚ÅŽlŠpŒ`
  //  vertices[0] = { Vector3(-hw,  hh, 0.0f), color };
  //  vertices[1] = { Vector3(hw,  hh, 0.0f), color };
  //  vertices[2] = { Vector3(-hw, -hh, 0.0f), color };
  //  vertices[3] = { Vector3(-hw, -hh, 0.0f), color };
  //  vertices[4] = { Vector3(hw,  hh, 0.0f), color };
  //  vertices[5] = { Vector3(hw, -hh, 0.0f), color };

  //  mesh->SetVertices(vertices);
  //  mesh->CreateVertexBuffer(device);
  //  return mesh;
  //}

  //std::shared_ptr<Mesh> MeshFactory::CreateCircle(ID3D11Device* device, float radius, int segments, const Vector4& color)
  //{
  //  auto mesh = std::make_shared<Mesh>();
  //  std::vector<Vertex> vertices;
  //  for (int i = 0; i < segments; ++i) {
  //    // ’†S
  //    vertices.push_back({ Vector3(0.0f, 0.0f, 0.0f), color });

  //    // ‰~Žüã‚Ì2“_
  //    float theta0 = (2.0f * Math::Pi * i) / segments;
  //    float theta1 = (2.0f * Math::Pi * (i + 1)) / segments;
  //    float x0 = radius * std::cos(theta0);
  //    float y0 = radius * std::sin(theta0);
  //    float x1 = radius * std::cos(theta1);
  //    float y1 = radius * std::sin(theta1);

  //    vertices.push_back({ Vector3(x0, y0, 0.0f), color });
  //    vertices.push_back({ Vector3(x1, y1, 0.0f), color });
  //  }
  //  mesh->SetVertices(vertices);
  //  mesh->CreateVertexBuffer(device);
  //  mesh->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//return mesh;
  //}
}
