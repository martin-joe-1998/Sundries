#include "Pizza.h"
#include "Application/Application.h"

#include "Utility/Logger.h"
#include "Data/Config.h"

namespace sl
{
	Pizza::Pizza(Game* game)
		: Actor(game)
	{
		SetScale(Vector3(1.0f, 1.0f, 0.f));

		mMeshComponent = std::make_shared<MeshComponent>(this, MeshType::EPizza);
		// Blend state
		// Ôª¤È¤Ê¤Ã¤Æ¤¤¤ë¥Ô¥¶¤ò°ëÍ¸Ã÷¤Ë¤¹¤ë
		SetRenderType("Transparent");
	}

	void Pizza::UpdateActor(float deltaTime)
	{
		POINT point;
		if (GetCursorPos(&point))
		{
			ScreenToClient(Application::GetInstance().GetWindowHandle(), &point);
			Vector2 cursorDir = Vector2(point.x, point.y) -
				Vector2(Application::GetInstance().GetWindowWidth() * 0.5,
					Application::GetInstance().GetWindowHeight() * 0.5);
			float angle = atan2(cursorDir.y, cursorDir.x);
			angle = Math::ToDegrees(angle);
			angle = (angle < 0) ? (angle + 360.f) : angle;

			mSectorNum = (angle - Config::Note::LaneAngle) / Config::Note::AngleOffset; // angle‚ª67.5–¢–ž‚¾‚Æ6C7‚Å‚Í‚È‚­-1,0‚É‚È‚Á‚Ä‚µ‚Ü‚¤

			// Todo: –³—‚â‚è‚·‚¬‚é
			if (angle >= 0 && angle < Config::Note::LaneAngle * 0.5)
			{
				mSectorNum = Config::Note::SectorNum - 2;
			}
			else if (angle >= Config::Note::LaneAngle * 0.5 && angle <= Config::Note::AngleOffset)
			{
				mSectorNum = Config::Note::SectorNum - 1;
			}

			SetSectorNumInMeshComponent(mSectorNum);
		}
	}
}
