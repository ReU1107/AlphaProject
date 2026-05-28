#include "Assertion.h"
#include <Windows.h>

namespace Alpha
{
	void BreakDebug()
	{
		if (::IsDebuggerPresent())	// C++26移行になったらhttps://cpprefjp.github.io/reference/debugging.html こっちに変える
		{
			__debugbreak();
		}
	}
}