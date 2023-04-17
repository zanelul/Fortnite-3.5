#pragma once
#include <Windows.h>
#include <memory>

namespace Hooks {
	void VftTableSwap(PVOID Interface, int VftIndex, PVOID Function, PVOID* Original);
}