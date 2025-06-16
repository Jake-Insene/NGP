/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/Window.h"

#include "Platform/Header.h"
#include "IO/Pad/Pad.h"

Pad::PadButton buttons_map[256] = {};

static inline Pad::PadButton _wp_to_buttons(WPARAM wp) {
    return buttons_map[wp & 0xFF];
}

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
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
    case WM_KEYUP:
        Pad::update(0, _wp_to_buttons(wp), msg == WM_KEYDOWN);
        break;
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

void Window::initialize(i32 width, i32 height)
{
    buttons_map['A'] = Pad::PAD_LEFT;
    buttons_map['D'] = Pad::PAD_RIGHT;
    buttons_map['W'] = Pad::PAD_UP;
    buttons_map['S'] = Pad::PAD_DOWN;

    buttons_map['J'] = Pad::PAD_X;
    buttons_map['L'] = Pad::PAD_Y;
    buttons_map['I'] = Pad::PAD_Z;
    buttons_map['K'] = Pad::PAD_W;

    buttons_map['Z'] = Pad::PAD_SELECT;
    buttons_map['X'] = Pad::PAD_START;
    buttons_map['Q'] = Pad::PAD_LB;
    buttons_map['E'] = Pad::PAD_RB;

    current_width = width;
    current_height = height;

    static bool is_register = false;
    if (!is_register)
    {
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

    RECT desktop_rect = {};
    GetWindowRect(GetDesktopWindow(), &desktop_rect);

    RECT window_rect = {};
    window_rect.right = width;
    window_rect.bottom = height;
    AdjustWindowRectEx(&window_rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

    i32 centerX = ((desktop_rect.right - desktop_rect.left) / 2) - (width / 2);
    i32 centerY = ((desktop_rect.bottom - desktop_rect.top) / 2) - (height / 2);

    handle = CreateWindowExA(
        WS_EX_OVERLAPPEDWINDOW, "ngpw", "NGP", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        centerX, centerY, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
        nullptr, nullptr, nullptr, nullptr
    );

    is_open = true;
}

void Window::shutdown()
{
    DestroyWindow((HWND)handle);
}

void Window::update()
{
    MSG msg = {};
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) == TRUE)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

