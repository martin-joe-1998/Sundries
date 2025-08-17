#pragma once
#include "Game/Actor.h"
#include "Utility/Math.h"


namespace sl
{
	class Game;
	class MeshComponent;

	class LaneLine : public Actor
	{
	public:
		LaneLine(Game* game, float angle);
		~LaneLine() {};

		void UpdateActor(float deltaTime) override;

	private:
		std::shared_ptr<MeshComponent> mMeshComponent;
	};

}