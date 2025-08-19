#include "Spectrum.h"
#include "Game/Components/MeshComponent.h"
#include "Utility/Logger.h"

namespace sl
{
	Spectrum::Spectrum(Game* game)
		: Actor(game)
		, mTimer(0.0f)
	{
		SetScale(Vector3(1.0f, 1.0f, 1.0f));

		mMeshComponent = std::make_shared<MeshComponent>(this, MeshType::ESpectrum);

		SetRenderType("Opaque");
	}

	void Spectrum::UpdateActor(float deltaTime)
	{
		mTimer += deltaTime;
	}
}