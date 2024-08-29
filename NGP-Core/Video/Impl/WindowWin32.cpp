/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Platform/Header.h"
#include "Video/Window.h"
#include "IO/Pad.h"

IO::PadButton buttons_map[256] = {};

static inline IO::PadButton _wp_to_buttons(WPARAM wp) {
    return buttons_map[wp & 0xFF];
}

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CLOSE:
        Window::is_open = false;
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        return 0;
    case WM_SIZE:
        Window::current_height = LOWORD(lp);
        Window::current_height = HIWORD(lp);
        break;
    case WM_KEYDOWN:
        IO::pad_update(0, _wp_to_buttons(wp), true);
        break;
    case WM_KEYUP:
        IO::pad_update(0, _wp_to_buttons(wp), false);
        break;
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

void Window::initialize(u32 width, u32 height)
{
    buttons_map['A'] = IO::PAD_LEFT;
    buttons_map['D'] = IO::PAD_RIGHT;
    buttons_map['W'] = IO::PAD_UP;
    buttons_map['S'] = IO::PAD_DOWN;

    buttons_map['J'] = IO::PAD_X;
    buttons_map['L'] = IO::PAD_Y;
    buttons_map['I'] = IO::PAD_Z;
    buttons_map['K'] = IO::PAD_W;

    buttons_map['Z'] = IO::PAD_SELECT;
    buttons_map['X'] = IO::PAD_START;
    buttons_map['Q'] = IO::PAD_LB;
    buttons_map['E'] = IO::PAD_RB;

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

void Window::shutdown() {
    DestroyWindow((HWND)handle);
}

void Window::update()
{
    MSG msg = {};
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

