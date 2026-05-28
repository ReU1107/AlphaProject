#pragma once
#include "Types.h"
#include "String/String.h"

#include <functional>

namespace Alpha
{
    using WindowEventCallback = std::function<void(UInt32 msg, intptr_t wParam, intptr_t lParam)>;
    using WindowDropFilesCallback = std::function<void(const std::vector<String>&)>;
    using WindowResizeCallback = std::function<void(UInt32 width, UInt32 height)>;

    class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual bool Initialize(
            const String& title,
            UInt32 width,
            UInt32 height,
            bool fullscreen = false) = 0;

        virtual void Finalize() = 0;

        // メッセージループ処理
        virtual bool ProcessMessages() = 0;

        // ウィンドウ情報取得
        virtual void* GetNativeHandle() const = 0;  // HWND/QMainWindow*
        virtual UInt32 GetWidth() const = 0;
        virtual UInt32 GetHeight() const = 0;
        virtual bool IsValid() const = 0;
        virtual bool ShouldClose() const = 0;

        // イベントコールバック登録
        virtual void SetEventCallback(WindowEventCallback callback) = 0;
        virtual void SetDropFilesCallback(WindowDropFilesCallback callback) = 0;
        virtual void SetResizeCallback(WindowResizeCallback callback) = 0;

        // ウィンドウ操作
        virtual void SetTitle(const String& title) = 0;
        virtual void SetSize(UInt32 width, UInt32 height) = 0;
        virtual void SetFullscreen(bool fullscreen) = 0;
        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void RequestClose() = 0;
    };

}