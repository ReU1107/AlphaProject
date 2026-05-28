#pragma once
#include "IWindow.h"

#if _WIN32
#include "Windows.h"

namespace Alpha
{
	class Win32Window : public IWindow
	{
    private:
        HWND m_hwnd{ nullptr };
        UInt32 m_width{ 1920 };
        UInt32 m_height{ 1080 };
        bool m_should_close{ false };

        // コールバック
        WindowEventCallback m_event_callback{};
        WindowDropFilesCallback m_drop_files_callback{};
        WindowResizeCallback m_resize_callback{};

        // 静的メンバ（ウィンドウプロシージャから参照）
        static Win32Window* s_instance;
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    public:
        Win32Window();
        ~Win32Window();

        bool Initialize(
            const String& title,
            UInt32 width,
            UInt32 height,
            bool fullscreen = false) override;

        void Finalize() override;

        bool ProcessMessages() override;

        void* GetNativeHandle() const override { return m_hwnd; }
        UInt32 GetWidth() const override { return m_width; }
        UInt32 GetHeight() const override { return m_height; }
        bool IsValid() const override { return m_hwnd != nullptr; }
        bool ShouldClose() const override { return m_should_close; }

        void SetEventCallback(WindowEventCallback callback) override {
            m_event_callback = callback;
        }

        void SetDropFilesCallback(WindowDropFilesCallback callback) override {
            m_drop_files_callback = callback;
        }

        void SetResizeCallback(WindowResizeCallback callback) override {
            m_resize_callback = callback;
        }

        void SetTitle(const String& title) override;
        void SetSize(UInt32 width, UInt32 height) override;
        void SetFullscreen(bool fullscreen) override;
        void Show() override;
        void Hide() override;
        void RequestClose() override { m_should_close = true; }

    private:
        void OnWndProc(UINT msg, WPARAM wParam, LPARAM lParam);
    };

}
#endif
