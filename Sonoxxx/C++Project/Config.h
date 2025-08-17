#pragma once

namespace Config
{
	namespace InGame
	{
		static constexpr int StartWaitTime = 15.0f; // RhythmSceneがロードされてから曲を再生するまでの時間
	}

	namespace Note
	{
		static constexpr int SectorNum = 8; // レーン分割数
		static constexpr float LaneAngle = 360.0 / SectorNum; // 1つのレーンの角度
		static constexpr float AngleOffset = LaneAngle * 1.5; // 真上がレーンの真ん中になるようなオフセット
		static constexpr float MaxRadius = 0.45f; // ノーツの最大移動距離

		static constexpr float Speed = 0.2f; // ノーツの移動速度
		static constexpr float NoteLifeTime = 5.0f; // １つのノーツの生存時間

	}

	namespace JudgeTime
	{
		static constexpr float CriteriaTime = 1.5f; // ノーツを発射してから判定までの時間。完璧なタイミング
		// 各判定までの時間差
		static constexpr float Perfect = 0.03333f;
		static constexpr float Great = 0.06666f;
		static constexpr float Good = 0.1f;
		static constexpr float Miss = 0.35f;
	}

	namespace Score
	{
		static constexpr int Perfect = 100;
		static constexpr int Great = 50;
		static constexpr int Good = 10;
	}

	namespace Common
	{
		static constexpr float BackGroundColor[] = { 0.02f, 0.02f, 0.02f, 1.f };
		static constexpr float BlackBackGround[] = { 0.0f, 0.0f, 0.0f, 1.f };
		//static constexpr int WindowWidth = 1280;
		//static constexpr int WindowHeight = 720;
	}
	
}

