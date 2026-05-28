#pragma once
#include "Types.h"
#include "Subsystem.h"
#include "String/String.h"
#include "SmartPtr.h"

#include "IWindow.h"

#include <functional>

namespace Alpha
{
    class WindowSystem : public Subsystem<WindowSystem>
    {
    private:
        TUnique<IWindow> m_win{};

    public:
        WindowSystem();
        ~WindowSystem();
        struct Args
        {
            String title = L"Application";
            UInt32 width = 1920;
            UInt32 height = 1080;
            bool fullscreen = false;
        };

        bool Initialize(const Args& args);
        void Finalize();

        // デリゲート
        IWindow* GetWindow() { return m_win.get(); }
        const IWindow* GetWindow() const { return m_win.get(); }

        // 直接アクセス（便利メソッド）
        void* GetNativeHandle() const { return m_win->GetNativeHandle(); }
        UInt32 GetWidth() const { return m_win->GetWidth(); }
        UInt32 GetHeight() const { return m_win->GetHeight(); }
        bool IsValid() const { return m_win->IsValid(); }
        bool ShouldClose() const { return m_win->ShouldClose(); }
        bool ProcessMessages() { return m_win->ProcessMessages(); }

        void SetTitle(const String& title) { m_win->SetTitle(title); }
        void SetSize(UInt32 w, UInt32 h) { m_win->SetSize(w, h); }
        void RequestClose() { m_win->RequestClose(); }

        void SetEventCallback(WindowEventCallback cb) { m_win->SetEventCallback(cb); }
        void SetDropFilesCallback(WindowDropFilesCallback cb) { m_win->SetDropFilesCallback(cb); }
        void SetResizeCallback(WindowResizeCallback cb) { m_win->SetResizeCallback(cb); }
    };

}