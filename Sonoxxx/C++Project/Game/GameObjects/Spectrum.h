#pragma once
#include "Game/Actor.h"

namespace sl
{
	class Game;
	class SceneBase;
	class MeshComponent;

	class Spectrum : public Actor
	{
	public:
		Spectrum(Game* game);
		~Spectrum() {}

		void UpdateActor(float deltaTime) override;

	private:
		std::shared_ptr<MeshComponent> mMeshComponent;
		float mTimer;
	};
};

