// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"
class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const struct InputState& state) override;
	void SetControllerIndex(int index) { mControllerIndex = index; }
	int GetControllerIndex() const { return mControllerIndex; }
private:
	Vector2 mVelocityDir;
	Vector2 mRotationDir;
	float mSpeed;
	float mLaserCooldown;
	// **
	// Default no controller
	int mControllerIndex = -1;
};