#include "Window.h"

LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY ||
		msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, msg, wparam, lparam);
}

Window::Window(int width, int height)
{
	// Define window style
	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC; // Use own device context
	wc.lpfnWndProc = WinProc; // Callback function
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = "OpticalDX11";
	RegisterClass(&wc);

	// Create a window class structure
	m_handle = CreateWindow(
		"OpticalDX11", "Optical Ⅰ", 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
		100, 100, width, height,
		nullptr, nullptr, nullptr, nullptr );

}