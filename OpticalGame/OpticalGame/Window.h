#pragma once
#include <Windows.h>

class Window
{
public:
	Window(int width, int height);
	~Window() {}
	HWND GetHandle() { return m_handle; }

private:
	HWND m_handle;
};