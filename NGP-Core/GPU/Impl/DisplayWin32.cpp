// --------------------
// DisplayWin32.cpp
// --------------------
// Copyright (c) 2024 jake
// See the LICENSE in the project root.
#include "GPU/Display.h"
#include <Windows.h>

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CLOSE:
        Display::is_open = false;
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        return 0;
    case WM_SIZE:
        Display::current_height = LOWORD(lp);
        Display::current_height = HIWORD(lp);
        break;
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

void Display::initialize(u32 width, u32 height)
{
    current_width = width;
    current_height = height;

    static bool is_register = false;
    if (!is_register) {
        WNDCLASSEXA wc = {
            .cbSize = sizeof(WNDCLASSEXA),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = wnd_proc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GetModuleHandleA(nullptr),
            .hIcon = LoadIcon(nullptr, IDI_APPLICATION),
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = 0,
            .lpszMenuName = nullptr,
            .lpszClassName = "ngpw",
            .hIconSm = LoadIcon(nullptr, IDI_APPLICATION),
        };

        RegisterClassExA(&wc);
        is_register = true;
    }

    RECT rect = {};
    GetWindowRect(GetDesktopWindow(), &rect);

    i32 centerX = ((rect.right - rect.left) / 2) - (width / 2);
    i32 centerY = ((rect.bottom - rect.top) / 2) - (height / 2);

    handle = CreateWindowExA(
        WS_EX_APPWINDOW, "ngpw", "NGP", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        centerX, centerY, width, height,
        nullptr, nullptr, nullptr, nullptr
    );

    is_open = true;
}

void Display::shutdown() {
    DestroyWindow((HWND)handle);
}

void Display::update()
{
    MSG msg = {};
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

