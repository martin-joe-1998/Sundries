#pragma once
#include "Component.h"

#include "Common.h"
#include "Graphics/Material.h"

namespace sl
{
	/// <summary>
	/// ���ۃN���X�ł��B�Q�[���I�u�W�F�N�g�̕`��ɕK�v�ȏ����܂Ƃ߂܂����B�p�����Ďg��
	/// �V�F�[�_�x�[�X�̏ꍇ��MeshComponent���p�����A�e�N�X�`���x�[�X�̏ꍇ��SpriteComponent���p������B
	/// Material���V�F�[�_��e�N�X�`���������Ă���̂ŁA�h���N���X�ł��������ɃZ�b�g����
	/// 
	/// �����͂��Ȃ��Ȃ�\��
	/// 
	/// </summary>
	class RenderComponent :	public Component
	{
	public:
		RenderComponent(Actor* owner, const Material& material)
			: Component(owner), mMaterial(material) {	}
		virtual ~RenderComponent() = default;

		/// <summary>
		/// �`��ɕK�v�ȏ����擾����B
		/// Scene�Ȃǂŕ`�悷��ۂɁA�eRenderComponent���炱�̏����擾
		/// �h����ł����ƃ��[���h�s����v�Z���ē���ĂˁI�I�I�I�I�I
		/// </summary>
		virtual DrawDesc GetDrawDesc() const = 0;
		const Material& GetMaterial() const { return mMaterial; }

	private:
		Material mMaterial; // �`��ɕK�v�ȃ}�e���A�����i�V�F�[�_�A�e�N�X�`���Ȃǁj
		// Mesh��Texture�Ȃǂ̃��\�[�X�������ŊǗ����邱�Ƃ��ł���
		// �Ⴆ�΁AMeshComponent�Ȃ�Mesh�����ASpriteComponent�Ȃ�Texture�����Ȃ�
		// �������ARenderComponent�͒��ۃN���X�Ȃ̂ŁA��̓I�Ȏ����͔h���N���X�ōs��
	};
}

