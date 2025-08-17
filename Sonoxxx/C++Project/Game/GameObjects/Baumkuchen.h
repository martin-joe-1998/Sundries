#pragma once
#include "Game/Actor.h"
#include "Data/FileIO.h"

namespace sl
{
	class Game;
	class MeshComponent;

	// NodeManagerを早くつくろう
	enum class JudgementCriteria
	{
		EPerfect, // 33.33ms
		EGreat, // 66.66
		EGood, // 100.00
		EMiss,
		ENone
	};
	
	class Baumkuchen : public Actor
	{
	public:
		Baumkuchen(Game* game);
		~Baumkuchen() {}

		void UpdateActor(float delataTime) override;
		
		JudgementCriteria Judge();

		float GetTime() { return mTimer; }
		int GetLaneNum() { return mLaneNum; }
		void SetTime(float value) { mTimer = value; }
		void SetLaneNum(int value) { mLaneNum = value; }

		float EvaluateProximity(); // 判定とどれだけ近いかを返す

		nlohmann::json ToJson() const {
			nlohmann::json json;
			JsonHelper::AddFloat(json, "time", mTimer);
			return json;
		}

	private:
		std::shared_ptr<MeshComponent> mMeshComponent;
		int mLaneNum = 0;
		float mAngle = 90.0f;
		float mCurrentRadius;
		float mTimer;

		//const float mMaxRadius = 0.45f;
		//const int mSectorNum = 8;


		//const float mCriteriaTime = 1.5f; // 完璧なタイミング Todo: 決め打ちすぎる
		//// Timerは別で管理するべき
		//// チュウニズム参考
		//const float mPerfectTimediff = 0.03333f;
		//const float mGreatTimeDiff = 0.06666f;
		////const float mGoodTimeDiff = 0.1f;
		//const float mGoodTimeDiff = 0.1f;
		//const float mMissTimeDiff = 0.35f;

		float CalcAngle(int laneNum);
	};

}
