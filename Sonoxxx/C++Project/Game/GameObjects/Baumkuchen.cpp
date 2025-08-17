#include "Baumkuchen.h"

#include "Game/Components/MeshComponent.h"

#include "Utility/Logger.h"
#include "Data/Config.h"

namespace sl
{
	Baumkuchen::Baumkuchen(Game* game)
		: Actor(game)
		, mCurrentRadius(0.0f)
		, mTimer(0.0f)
	{
		SetScale(Vector3(1.0f, 1.0f, 0.f));

		mMeshComponent = std::make_shared<MeshComponent>(this, MeshType::EBaumkuchen);

		// Blend state
		// ‘™§»§ §√§∆§§§Î•‘•∂§Ú∞ÎÕ∏√˜§À§π§Î
		SetRenderType("Transparent");
	}

	void Baumkuchen::UpdateActor(float delataTime)
	{
		float speed = 0.2f;

		if (mCurrentRadius >= Config::Note::MaxRadius)
		{
			//LOG_DEBUG("miss", mTimer);
			//mMeshComponent->SetColor(Color::Black);
			//Reset();
		}
		else
		{
			mTimer += delataTime;
			mCurrentRadius += speed * delataTime;
			mMeshComponent->SetRadius(mCurrentRadius);

			// debug
			if (abs(mTimer - Config::JudgeTime::CriteriaTime) <= Config::JudgeTime::Perfect)
			{
				mMeshComponent->SetColor(Color::White);
				//LOG_INFO("Perfect");
			}
			else if (abs(mTimer - Config::JudgeTime::CriteriaTime) <= Config::JudgeTime::Great)
			{
				mMeshComponent->SetColor(Color::Red);
				//LOG_INFO("Great");
			}
			else if (abs(mTimer - Config::JudgeTime::CriteriaTime) <= Config::JudgeTime::Good)
			{
				mMeshComponent->SetColor(Color::Cyan);
				//LOG_INFO("Good");
			}
			else if (abs(mTimer - Config::JudgeTime::CriteriaTime) <= Config::JudgeTime::Miss)
			{
				mMeshComponent->SetColor(Color::Magenta);
				//LOG_INFO("Miss ^ ^ ");
			}
			else
			{
				mMeshComponent->SetColor(Color::Blue);
			}
			//LOG_INFO("FPS: ", 1 / delataTime);
		}

		mMeshComponent->SetAngleOffset(CalcAngle(mLaneNum));
	}

	JudgementCriteria Baumkuchen::Judge()
	{
		float timeDiff = abs(mTimer - Config::JudgeTime::CriteriaTime);
		if (timeDiff <= Config::JudgeTime::Perfect)
		{
			return JudgementCriteria::EPerfect;
		}
		else if (timeDiff > Config::JudgeTime::Perfect && timeDiff <= Config::JudgeTime::Great)
		{
			return JudgementCriteria::EGreat;
		}
		else if (timeDiff > Config::JudgeTime::Great && timeDiff <= Config::JudgeTime::Good)
		{
			return JudgementCriteria::EGood;
		}
		else if (timeDiff > Config::JudgeTime::Good && timeDiff <= Config::JudgeTime::Miss)
		{
			return JudgementCriteria::EMiss;
		}
		else
		{
			return JudgementCriteria::ENone;
		}
	}

	float Baumkuchen::EvaluateProximity()
	{
		return abs(mTimer - Config::JudgeTime::CriteriaTime);
	}

	float Baumkuchen::CalcAngle(int laneNum)
	{
		if (laneNum < 0 || laneNum > Config::Note::SectorNum - 1) { LOG_WARN("ÉåÅ[ÉìîÕàÕäOÇ≈Ç∑ÇÊ"); }
		return laneNum * (360.f / Config::Note::SectorNum);
	}
}