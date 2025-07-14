#pragma once
#include "RenderComponent.h"
#include <memory>
#include "Graphics/Mesh.h"
#include "Game/Actor.h"

namespace sl
{
  /// <summary>
	/// �V�F�[�_�[�x�[�X�̕`��R���|�[�l���g�B
  /// �ɁA������
  /// </summary>
  class MeshComponent : public RenderComponent
  {
  public:
    MeshComponent(Actor* owner, const Material& material)
      : RenderComponent(owner, material) {
    }

    void SetMesh(const std::shared_ptr<Mesh>& mesh) { mMesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return mMesh; }

    // �`�����Ԃ�
    virtual DrawDesc GetDrawDesc() const override
    {
      DrawDesc desc;
      const Vector3& pos = mOwner->GetPosition();
      const Vector3& rot = mOwner->GetRotation();
      const Vector3& scale = mOwner->GetScale();
      desc.worldMatrix = Math::MakeWorldMatrix(pos, rot, scale);

      desc.material = &GetMaterial();
      desc.mesh = mMesh.get(); // ������ǉ�

      return desc;
    }

    void Draw(ID3D11DeviceContext* context)
    {
      //if (mMesh) mMesh->Draw(context);
    }

  private:
    std::shared_ptr<Mesh> mMesh;
  };
}
