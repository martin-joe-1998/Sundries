#pragma once
#include <Windows.h>

class Window
{
public:
	Window(int width, int height);
	HWND GetHandle() { return m_handle; }

private:
	HWND m_handle;
};