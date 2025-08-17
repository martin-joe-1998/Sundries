#pragma once
#include "Game/Actor.h"
#include "Game/Components/MeshComponent.h"

namespace sl
{
	class Game;
	class MeshComponent;

	class Pizza : public Actor
	{
	public:

		Pizza(Game* game);

		~Pizza() {}

		void UpdateActor(float deltaTime) override;
		int GetSectorNum() { return mSectorNum; }

	private:
		std::shared_ptr<MeshComponent> mMeshComponent;
		int mSectorNum = 0;

		void SetSectorNumInMeshComponent(int num) { mMeshComponent->SetSectorNum(num); }

	};

}