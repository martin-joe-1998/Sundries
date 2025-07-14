#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <Utility/Math.h>

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

namespace sl
{
	struct BaseDesc
	{
	};

	// SE�̍Đ��ʒu�A�����ǂ��ɕ`�悷�邩�AScene�̏�ԂȂǁA
	struct GameDesc : public BaseDesc
	{
		// SceneState {�J�ڂ��邩�A�|�[�Y���Ƃ�}
		// AudioPlayState {SE�̍Đ��ʒu�ABGM�̍Đ���ԂȂ�}
		// RendererDesc Renderer; // �`��Ɋւ���ݒ�
		// InputManagerDesc Input; // ���͂Ɋւ���ݒ�
		// ResourceManagerDesc Resource; // ���\�[�X�Ɋւ���ݒ�
		// UIManagerDesc UI; // UI�Ɋւ���ݒ�

		bool isQuit = false; // �Q�[���I���t���O
	};

	/// <summary>
	/// �P�̃Q�[���I�u�W�F�N�g���`��Ɋւ��ĕK�v�ȏ����܂Ƃ߂��\���́B
	/// </summary>
	struct DrawDesc : public BaseDesc
	{
		Matrix4 worldMatrix;
		const Material* material;
		const Mesh* mesh;
	};
}