// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem.h"
#include <SDL/SDL.h>
#include <cstring>

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons);
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool InputSystem::Initialize()
{
	// Keyboard
	// Assign current state pointer
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// Clear previous state memory
	memset(mState.Keyboard.mPrevState, 0,
		SDL_NUM_SCANCODES);

	// Mouse (just set everything to 0)
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;

	// **
	for (int i = 0; i < MAX_CONTROLLERS; ++i)
	{
		mState.Controller[i].mController = nullptr;
		mState.Controller[i].mIsConnected = false;
		memset(mState.Controller[i].mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
		memset(mState.Controller[i].mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
	}

	// Try to open up to MAX_CONTROLLERS game controllers
	int controllerIndex = 0;
	int joystickCount = SDL_NumJoysticks();
	for (int i = 0; i < joystickCount && controllerIndex < MAX_CONTROLLERS; ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller)
			{
				mState.Controller[controllerIndex].mController = controller;
				mState.Controller[controllerIndex].mIsConnected = true;
				++controllerIndex;
			}
		}
	}

	/*
	// Get the connected controller, if it exists
	mController = SDL_GameControllerOpen(0);
	// Initialize controller state
	mState.Controller.mIsConnected = (mController != nullptr);
	memset(mState.Controller.mCurrButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
	memset(mState.Controller.mPrevButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
	*/

	return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
	// Copy current state to previous
	// Keyboard
	memcpy(mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES);

	// Mouse
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	mState.Mouse.mIsRelative = false;
	mState.Mouse.mScrollWheel = Vector2::Zero;

	// **
	// Controller
	for (int i = 0; i < MAX_CONTROLLERS; ++i)
	{
		if (mState.Controller[i].mIsConnected)
		{
			memcpy(mState.Controller[i].mPrevButtons,
				mState.Controller[i].mCurrButtons,
				SDL_CONTROLLER_BUTTON_MAX);
		}
	}
	
}

void InputSystem::Update()
{
	// Mouse
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetMouseState(&x, &y);
	}

	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	// Controller
	/*
	// Buttons
	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	{
		mState.Controller.mCurrButtons[i] =
			SDL_GameControllerGetButton(mController, 
				SDL_GameControllerButton(i));
	}

	// Triggers
	mState.Controller.mLeftTrigger =
		Filter1D(SDL_GameControllerGetAxis(mController,
			SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	mState.Controller.mRightTrigger =
		Filter1D(SDL_GameControllerGetAxis(mController,
			SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

	// Sticks
	x = SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_LEFTX);
	y = -SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_LEFTY);
	mState.Controller.mLeftStick = Filter2D(x, y);

	x = SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_RIGHTX);
	y = -SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_RIGHTY);
	mState.Controller.mRightStick = Filter2D(x, y);
	*/
	for (int i = 0; i < MAX_CONTROLLERS; ++i)
	{
		ControllerState& cs = mState.Controller[i];

		if (cs.mIsConnected && cs.mController != nullptr)
		{
			// Record button state at last frame
			memcpy(cs.mPrevButtons, cs.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);

			// Button
			for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++)
			{
				cs.mCurrButtons[b] = SDL_GameControllerGetButton(
					cs.mController,
					static_cast<SDL_GameControllerButton>(b)
				);
			}

			// Trigger
			cs.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
			cs.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

			// Left stick
			x = SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_LEFTX);
			y = -SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_LEFTY);
			cs.mLeftStick = Filter2D(x, y);

			// Right stick
			x = SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_RIGHTX);
			y = -SDL_GameControllerGetAxis(cs.mController, SDL_CONTROLLER_AXIS_RIGHTY);
			cs.mRightStick = Filter2D(x, y);
		}
	}
}

// **
void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y));
		break;
	case SDL_CONTROLLERDEVICEADDED:
	{
		int deviceIndex = event.cdevice.which;

		for (int i = 0; i < MAX_CONTROLLERS; ++i)
		{
			if (!mState.Controller[i].mIsConnected)
			{
				SDL_GameController* controller = SDL_GameControllerOpen(deviceIndex);
				if (controller)
				{
					mState.Controller[i].mController = controller;
					mState.Controller[i].mIsConnected = true;
					SDL_Log("Controller connected at slot %d (SDL index %d)", i, deviceIndex);
				}
				break;
			}
		}
		break;
	}
	case SDL_CONTROLLERDEVICEREMOVED:
	{
		SDL_JoystickID removedID = event.cdevice.which;
		for (int i = 0; i < MAX_CONTROLLERS; ++i)
		{
			if (mState.Controller[i].mIsConnected &&
				SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(mState.Controller[i].mController)) == removedID)
			{
				SDL_GameControllerClose(mState.Controller[i].mController);
				mState.Controller[i].mController = nullptr;
				mState.Controller[i].mIsConnected = false;
				SDL_Log("Controller removed from slot %d (ID %d)", i, removedID);
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
		(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}
