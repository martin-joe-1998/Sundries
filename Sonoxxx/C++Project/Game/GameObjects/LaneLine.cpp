#include "LaneLine.h"
#include <windows.h>

#include "Game/Components/MeshComponent.h"
#include "Utility/Logger.h"
#include "Utility/Math.h"

namespace sl
{
	LaneLine::LaneLine(Game* game, float angle)
		: Actor(game)
	{
		SetScale(Vector3(0.7f, 0.7f, 0.2f));
		mMeshComponent = std::make_shared<MeshComponent>(this, MeshType::ELine);
		mMeshComponent->SetColor(Vector4(0.f, 1.f, 1.f, 1.f));

		Vector2 dir = Vector2(cos(angle), sin(angle));
		mMeshComponent->SetLineDir(dir);
	}

	void LaneLine::UpdateActor(float deltaTime)
	{

	}
}