#include <Windows.h>
#include "Application/Application.h"
#include "Utility/Math.h"
#include "Utility/Logger.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
  return sl::Application::GetInstance(hInstance, nCmdShow).Run();
}
