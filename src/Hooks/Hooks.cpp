#include "Hooks.h"

void Hooks::VftTableSwap(PVOID Interface, int VftIndex, PVOID Function, PVOID* Original) {
	void** VftTable = *(void***)Interface;

	int VftSize = 0;

	for (; VftTable[VftSize]; VftSize++) {
		if (IsBadCodePtr((FARPROC)VftTable[VftSize]))
			break;
	}

	auto NewVft = std::make_unique<void* []>(VftSize);
	std::memcpy(NewVft.get(), VftTable, VftSize * sizeof(void*));

	*Original = NewVft[VftIndex];
	NewVft[VftIndex] = Function;

	*(void***)(Interface) = NewVft.release();
}