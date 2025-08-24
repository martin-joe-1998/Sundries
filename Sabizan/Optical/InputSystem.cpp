#include "InputSystem.h"
#include <iostream>

InputSystem::InputSystem()
    : isGamepadConnected(false)
{
    InitializeInputState();
}

void InputSystem::InitializeInputState()
{
    ZeroMemory(&mCurr, sizeof(XINPUT_STATE));
    ZeroMemory(&mPrev, sizeof(XINPUT_STATE));

    DWORD r = XInputGetState(mUserIndex, &mCurr);
    isGamepadConnected = (r == ERROR_SUCCESS);
    if (isGamepadConnected) {
        printf("Gamepad %lu connected!\n", mUserIndex);
    }
    else {
        printf("Gamepad %lu undetected!\n", mUserIndex);
    }
}

InputSystem::GPState InputSystem::ReadGamepadSnapshot()
{
    GPState out{}; // 默认为全未按

    ZeroMemory(&mCurr, sizeof(XINPUT_STATE));
    DWORD r = XInputGetState(mUserIndex, &mCurr);
    if (r != ERROR_SUCCESS)
    {
        return out; // 未连接/读取失败 -> 全false
    }

    const WORD b = mCurr.Gamepad.wButtons;

    // D-Pad
    const bool dU = (b & XINPUT_GAMEPAD_DPAD_UP) != 0;
    const bool dD = (b & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    const bool dL = (b & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    const bool dR = (b & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

    // 左摇杆 -> 数字化（含死区+裕量，减少抖动）
    constexpr int DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE; // 7849
    constexpr int THRESH = DEADZONE + 3000;
    const SHORT lx = mCurr.Gamepad.sThumbLX;
    const SHORT ly = mCurr.Gamepad.sThumbLY;

    const bool lsL = (lx < -THRESH);
    const bool lsR = (lx > THRESH);
    const bool lsU = (ly > THRESH);
    const bool lsD = (ly < -THRESH);

    // 合并 D-Pad 与 左摇杆 到 WASD
    out.W = dU || lsU;
    out.S = dD || lsD;
    out.A = dL || lsL;
    out.D = dR || lsR;

    // A/B/触发器 -> J/K（可按你游戏语义调整）
    constexpr int TRIGGER_ON = 30;
    const bool A = (b & XINPUT_GAMEPAD_A) != 0;
    const bool B = (b & XINPUT_GAMEPAD_B) != 0;
    const bool RT = mCurr.Gamepad.bRightTrigger >= TRIGGER_ON;
    const bool LT = mCurr.Gamepad.bLeftTrigger >= TRIGGER_ON;

    out.J = A || RT; // 例如“确认/交互”
    out.K = B || LT; // 例如“取消/次要交互”

    return out;
}