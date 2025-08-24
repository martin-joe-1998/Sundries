#pragma once
#include <array>

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "Xinput9_1_0.lib")

class InputSystem
{
public:
    struct KeyState {
        int key = -1;
        bool isPressed = false;
        float timeSinceLastMove = 0.0f;
    };

    struct GPState {
        bool W = false, A = false, S = false, D = false; // Move
        bool J = false, K = false;                       // Interactive
    };

    static InputSystem& Instance() {
        static InputSystem instance;
        return instance;
    }

    static constexpr std::array<int, 4> moveKeyset = { 'W', 'A', 'S', 'D' };
    static constexpr std::array<int, 2> interactKeyset = { 'K', 'J' };

    void InitializeInputState();
    GPState ReadGamepadSnapshot();
    //void Update();

private:
    InputSystem();

    XINPUT_STATE mCurr{};
    XINPUT_STATE mPrev{};
    bool isGamepadConnected;
    DWORD mUserIndex = 0;
};

