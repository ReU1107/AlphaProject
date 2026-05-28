#include "Win32Window.h"

#if _WIN32
#include <windowsx.h>

namespace Alpha
{

    // 静的メンバ初期化
    Win32Window* Win32Window::s_instance = nullptr;

    Win32Window::Win32Window()
    {
        s_instance = this;
    }

    Win32Window::~Win32Window() 
    {
        s_instance = nullptr;
    }

    bool Win32Window::Initialize(const String& title, UInt32 width, UInt32 height, bool fullscreen)
    {
        m_width = width;
        m_height = height;

        // ウィンドウクラス登録
        WNDCLASS wc = {};
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hIcon = nullptr;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.lpszClassName = L"Alpha_WindowClass";

        if (!RegisterClass(&wc))
            return false;

        // ウィンドウサイズ計算
        RECT rect = { 0, 0, (LONG)width, (LONG)height };
        AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_ACCEPTFILES);

        int window_width = rect.right - rect.left;
        int window_height = rect.bottom - rect.top;

        // 画面中央に配置
        int screen_width = GetSystemMetrics(SM_CXSCREEN);
        int screen_height = GetSystemMetrics(SM_CYSCREEN);
        int pos_x = (screen_width - window_width) / 2;
        int pos_y = (screen_height - window_height) / 2;

        // ウィンドウ作成
        m_hwnd = CreateWindowEx(
            WS_EX_ACCEPTFILES,
            L"Alpha_WindowClass",
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            pos_x, pos_y,
            window_width, window_height,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );

        if (!m_hwnd)
            return false;

        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);

        return true;
    }

    void Win32Window::Finalize() 
    {
        if (m_hwnd)
        {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        UnregisterClass(L"Alpha_WindowClass", GetModuleHandle(nullptr));
    }

    bool Win32Window::ProcessMessages()
    {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
        {
            if (msg.message == WM_QUIT)
            {
                m_should_close = true;
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }

    void Win32Window::SetTitle(const String& title) 
    {
        if (m_hwnd)             
            SetWindowText(m_hwnd, title.c_str());

    }

    void Win32Window::SetSize(UInt32 width, UInt32 height)
    {
        if (m_hwnd) 
        {
            m_width = width;
            m_height = height;

            RECT rect = { 0, 0, (LONG)width, (LONG)height };
            AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_ACCEPTFILES);

            SetWindowPos(m_hwnd, nullptr,
                0, 0,
                rect.right - rect.left,
                rect.bottom - rect.top,
                SWP_NOMOVE);
        }
    }

    void Win32Window::SetFullscreen(bool fullscreen) 
    {
        // TODO: フルスクリーン実装
    }

    void Win32Window::Show() 
    {
        if (m_hwnd) ShowWindow(m_hwnd, SW_SHOW);
    }

    void Win32Window::Hide() 
    {
        if (m_hwnd) ShowWindow(m_hwnd, SW_HIDE);
    }

    LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (s_instance) {
            s_instance->OnWndProc(msg, wParam, lParam);
        }

        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_DROPFILES: {
            HDROP hDrop = (HDROP)wParam;
            UInt32 count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

            std::vector<String> files;
            for (UInt32 i = 0; i < count; ++i)
            {
                wchar_t buffer[512];
                DragQueryFile(hDrop, i, buffer, 512);
                files.push_back(String(buffer));
            }

            if (s_instance && s_instance->m_drop_files_callback)
            {
                s_instance->m_drop_files_callback(files);
            }

            DragAcceptFiles(hWnd, TRUE);
            return 0;
        }
        case WM_SIZE:
            if (s_instance) 
            {
                UInt32 w = GET_X_LPARAM(lParam);
                UInt32 h = GET_Y_LPARAM(lParam);
                s_instance->m_width = w;
                s_instance->m_height = h;
                if (s_instance->m_resize_callback) 
                {
                    s_instance->m_resize_callback(w, h);
                }
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        return 0;
    }

    void Win32Window::OnWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (m_event_callback) {
            m_event_callback(msg, wParam, lParam);
        }
    }

}

#endif