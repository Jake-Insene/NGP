#include "Graphics/Display.h"
#include <Windows.h>

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

void initialize_display()
{
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
}

LRESULT wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    Display* display = (Display*)GetWindowLongPtr(wnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CLOSE:
        display->is_open = false;
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        return 0;
    }

    return DefWindowProcA(wnd, msg, wp, lp);
}

Display::Display(i32 width, i32 height)
{
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
    SetWindowLongPtrA((HWND)handle, GWLP_USERDATA, (LONG_PTR)this);

    native_graphics_context = (void*)GetDC((HWND)handle);
}

Display::~Display()
{
    DestroyWindow((HWND)handle);
}

void Display::update()
{
    MSG msg = {};
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void Display::draw_text(i32 x, i32 y, char* text, u32 count)
{
    RECT source = {};
    GetClientRect((HWND)handle, &source);

    FillRect((HDC)native_graphics_context, &source, (HBRUSH)GetStockObject(BLACK_BRUSH));

    source.left = x;
    source.top = y;

    SetTextColor((HDC)native_graphics_context, RGB(255, 255, 255));
    SetBkColor((HDC)native_graphics_context, RGB(0, 0, 0));
    DrawTextA((HDC)native_graphics_context, text, count, &source, 0);
}
