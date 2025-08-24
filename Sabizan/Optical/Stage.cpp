#include "Stage.h"
#include "../Game.h"
#include "../Tilemap/Map.h"
#include "../LightRaySystem.h"
#include "../Utility/UndoStage.h"

#include "../Actor/SlimeActor.h"
#include "../Actor/RockActor.h"
#include "../Actor/FloorActor.h"
#include "../Actor/GoalActor.h"
#include "../Actor/LightEmitterActor.h"
#include "../Actor/LightRayActor.h"
#include "../Actor/MirrorActor.h"
#include "../Actor/CrystalActor.h"
#include "../Actor/SpriteActor.h"
#include "../Actor/UIActor.h"

#include <algorithm>
#include <iostream>

Stage::Stage(Game* game)
	: mGame(game)
	, mSlimeInitialCoord({-1, -1})
	, mGoalCoord({-1, -1})
	, mInitialLightEmitDir(LightEmitterActor::EmitDir::ENull)
	, mInitialLightEmitColor(Vector3(0.0f, 0.0f, 0.0f))
	, mDelayCount(0)
	, mLightRaySystem(nullptr)
{
	mGame->AddStage(this);
	mMap = std::make_unique<Map>();

	UndoStage::Instance().ClearRecord();
}

Stage::~Stage()
{
	UnloadData();

	//mGame->RemoveStage(this);
}

void Stage::UnloadData()
{
	// LightRayActorや各種の箱の参照を一個消す
	if (mLightRaySystem != nullptr)
	{
		mLightRaySystem->Unload();
	}

	// 全Actorの参照を解放する
	mSlimeActor = nullptr;
	mPushableActors.clear();
	mPendingActors.clear();
	mActors.clear();

	UndoStage::Instance().ClearRecord();
}

void Stage::ProcessInput(float deltaTime, std::vector<InputSystem::KeyState>& keyStates)
{
	// Precess every actor's input like Game.cpp
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EActive)
		{
			actor->ProcessInput(deltaTime, keyStates);
		}
	}

	if (GetAsyncKeyState('L') & 0x8000)
	{
		UndoStage::Instance().Undo(this);
	}
}

void Stage::Update(float deltaTime)
{
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)
	{
		pending->ComputeWorldTransform();
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	if (mLightRaySystem != nullptr)
	{
		mLightRaySystem->Update();
	}

	// printing for test
	//for (const auto& row : mMap->GetEntityMap()) {
	//    for (int id : row) {
	//        std::cout << id << " ";
	//    }
	//    std::cout << "\n";
	//}
	//std::cout << "\n";
}

void Stage::CreateActorFromTileMap()
{
	auto tilemap = mMap->GetTileMap();
	//auto tileset = mMap->GetTileSet();

	// TileSetとTileMapの情報を参照に、静的Actorを生成し、座標を設定する
	for (int h = 0; h < mMap->GetMapHeight(); ++h)
	{
		for (int w = 0; w < mMap->GetMapWidth(); ++w)
		{
			int tileID = tilemap[h][w];
			if (tileID >= 4)
				continue;

			std::shared_ptr<Actor> tileActor = CreateActorFromTileID(tileID);

			tileActor->Initialize();
			tileActor->SetTileCoord(h, w);
			tileActor->SetPosition(
				Vector3(
					-5.0f + w * 1.0f,
					 5.0f - h * 1.0f,
					 0.0f // 静的Actor のレイヤーは 0.0
				)
			);

			if (tileActor->GetType() == "GoalActor")
			{
				mGoalCoord = { w, h };
				continue;
			}
		}
	}
}

void Stage::CreateActorFromEntityMap()
{
	// 動的Actor の初期位置を EntityMap に設置
	// Slime
	mMap->SetEntityMap(mSlimeInitialCoord, 4);

	// EntityMap を参照
	auto entityMap = mMap->GetEntityMap();
	
	// EntityMap中の 動的Actor を tileCoord から生成
	for (int h = 0; h < mMap->GetMapHeight(); ++h)
	{
		for (int w = 0; w < mMap->GetMapWidth(); ++w)
		{
			int tileID = entityMap[h][w];
			if (tileID >= 4) // 0~3 は静的Actor
			{
				std::shared_ptr<Actor> tileActor = CreateActorFromTileID(tileID);
				tileActor->Initialize();

				tileActor->SetTileCoord(h, w);
				tileActor->SetPosition(
					Vector3(
						-5.0f + w * 1.0f,
						 5.0f - h * 1.0f,
						-0.1f // 動的Actor のレイヤーは -0.1
					)
				);
			}
		}
	}
}

std::shared_ptr<Actor> Stage::CreateActorFromTileID(int tileID)
{
	switch (tileID)
	{
	case 0:
	{
		// Rock
		std::shared_ptr<RockActor> rockActor;
		rockActor = std::make_shared<RockActor>(mGame);
		return rockActor;
	}
	case 1:
	{
		// Floor
		std::shared_ptr<FloorActor> floorActor;
		floorActor = std::make_shared<FloorActor>(mGame);
		return floorActor;
	}
	case 2:
	{
		// Goal
		std::shared_ptr<GoalActor> goalActor;
		goalActor = std::make_shared<GoalActor>(mGame);
		return goalActor;
	}
	case 3:
	{
		// LightEmitter
		std::shared_ptr<LightEmitterActor> lightEmitterActor;
		lightEmitterActor = std::make_shared<LightEmitterActor>(mGame);
		lightEmitterActor->SetEmitDir(mInitialLightEmitDir);
		lightEmitterActor->SetEmitLightColor(mInitialLightEmitColor);

		mLightRaySystem->AddGimmickActorRef(lightEmitterActor);

		return lightEmitterActor;
	}
	case 4:
	{
		// Slime
		std::shared_ptr<SlimeActor> slimeActor;
		slimeActor = std::make_shared<SlimeActor>(mGame);
		mSlimeActor = slimeActor;

		return slimeActor;
	}
	case 5:
	{
		// Mirror
		std::shared_ptr<MirrorActor> mirrorActor;
		mirrorActor = std::make_shared<MirrorActor>(mGame);
		mirrorActor->SetMirrorInfo(mMirrorInfos[0]);
		if (mMirrorInfos.size() > 1) {
			std::rotate(mMirrorInfos.begin(), mMirrorInfos.begin() + 1, mMirrorInfos.end());
		}

		return mirrorActor;
	}
	case 6:
	{
		// Crystal
		std::shared_ptr<CrystalActor> crystalActor;
		crystalActor = std::make_shared<CrystalActor>(mGame);
		crystalActor->SetCrystalColor(mCrystalColors[0]);
		if (mCrystalColors.size() > 1) {
			std::rotate(mCrystalColors.begin(), mCrystalColors.begin() + 1, mCrystalColors.end());
		}

		return crystalActor;
	}
	default:
		MessageBox(nullptr, L"Failed to Create Tile from ID.", L"Error", MB_OK | MB_ICONERROR);
		break;
	}

	return nullptr;
}

void Stage::AddActor(std::shared_ptr<Actor> actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Stage::RemoveActor(std::shared_ptr<Actor> actor)
{	
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

bool Stage::TryGetPushableActors(const Tile::TileCoord& tileCoord, std::shared_ptr<Actor>& retActor)
{
	for (auto actor : mPushableActors)
	{
		if (actor->GetTileCoord() == tileCoord)
		{
			//printf("actor tilecoord : (%d, %d), input tilecoord : (%d, %d)", actor->GetTileCoord().w, actor->GetTileCoord().h, tileCoord.w, tileCoord.h);
			retActor = actor;
			return true;
		}
	}

	return false;
}

void Stage::SetUpBackground(const std::vector<std::string>& textureFileNames, const std::string& shaderFileName)
{
	std::shared_ptr<SpriteActor> bg;
	bg = std::make_shared<SpriteActor>(mGame, textureFileNames, shaderFileName, 90);
	bg->SetRenderType("Opaque");
	bg->Initialize();
	bg->SetPosition(Vector3(-0.5f, 0.5f, 1.0f));
	bg->SetScale(Vector3(1600.0f, 900.0f, 1.0f));
}

void Stage::CreateDialogBox(const std::vector<std::string>& textureFileNames, const std::string& shaderName, const Vector3& pos, const Vector3& scale)
{
	std::shared_ptr<SpriteActor> dialogBox;
	dialogBox = std::make_shared<SpriteActor>(mGame, textureFileNames, shaderName, 90);
	dialogBox->SetRenderType("Transparent");
	dialogBox->Initialize();
	dialogBox->SetPosition(pos);
	dialogBox->SetScale(scale);
}

std::shared_ptr<UIActor> Stage::CreateText(const char* text, const Vector3& pos, const float& scale)
{
	auto uiActor = std::make_shared<UIActor>(mGame, text, "UIShader.fxh");
	uiActor->Initialize();
	uiActor->SetPosition(pos);
	// 文字数でスケールを決める
	uiActor->SetScale(
		Vector3(scale * std::strlen(text), scale, 1.0f)
	);

	return uiActor;
}

// リセットに呼ばれる、文字を表示用の関数
bool Stage::ResetEffect()
{
	if (mDelayCount == 0)
	{
		std::shared_ptr<UIActor> actor;
		actor = CreateText("RESET!", Vector3(-0.5f, 0.5f, -2.0f), 60.0f);
		actor->SetFontColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	}

	if (mDelayCount++ >= 30)
	{
		printf("RESET!\n");
		mDelayCount = 0;

		return true;
	}

	return false;
}