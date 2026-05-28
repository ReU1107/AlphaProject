#include "WindowSystem.h"

#include "Win32Window.h"

namespace Alpha
{
	WindowSystem::WindowSystem() = default;
	WindowSystem::~WindowSystem() = default;

	bool WindowSystem::Initialize(const Args& args)
	{
#if _WIN32
		m_win = std::make_unique<Win32Window>();
#endif

		return m_win->Initialize(args.title, args.width, args.height, args.fullscreen);
	}

	void WindowSystem::Finalize()
	{
		m_win->Finalize();
	}
}