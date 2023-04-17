#pragma once
#include <Windows.h>

namespace Settings {
	inline int CurrentTab = 1;

	namespace Aim {
		inline bool EnableAim = true;
		inline bool AimLine = true;
		inline float AimRadius = 50.f;
	}
	namespace Visuals {
		inline bool SkeletonESP = true;
		inline bool BoxESP = true;
		inline bool LineESP = false;
		inline bool NameESP = false;
		inline float ESPDistance = 100.f;
	}
	namespace Exploits {
		inline bool FOVChanger = false;
		inline float FOVValue = 80.f;
	}
}