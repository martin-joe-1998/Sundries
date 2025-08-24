#pragma once
#include <Windows.h>

class Window
{
public:
	Window(int width, int height);
	~Window() {}
	HWND GetHandle() { return m_handle; }
	const int& GetWindowWidth() const { return mWindowWidth; }
	const int& GetWindowHeight() const { return mWindowHeight; }

private:
	HWND m_handle;
	int mWindowWidth;
	int mWindowHeight;
};