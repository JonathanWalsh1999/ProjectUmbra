#pragma once


#include "DirectX11Engine.hpp"

// Identifiers for the window that will show our app
bool InitWindow(HINSTANCE& hInstance, int nCmdShow, HWND& hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
