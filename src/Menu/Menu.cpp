#include "Menu.h"

void Menu::Render(SDK::UCanvas* Canvas) {
	zgui::drawlist::SetCanvas(Canvas);
	zgui::PollInput("Fortnite  ");
	zgui::BeginWindow("fortnite", SDK::FVector2D(400.f, 400.f), VK_F2); {
		if (zgui::Button("aim", SDK::FVector2D(116.f, 30.f))) Settings::CurrentTab = 1;
		zgui::SameLine();
		if (zgui::Button("visuals", SDK::FVector2D(116.f, 30.f))) Settings::CurrentTab = 2;
		zgui::SameLine();
		if (zgui::Button("exploits", SDK::FVector2D(116.f, 30.f))) Settings::CurrentTab = 3;

		zgui::SameLine(0.f);

		switch (Settings::CurrentTab) {
		case 1:
			zgui::Checkbox("enable", &Settings::Aim::EnableAim);
			zgui::Checkbox("aim line", &Settings::Aim::AimLine);
			zgui::SliderFloat("radius", 5.f, 360.f, &Settings::Aim::AimRadius);
			break;
		case 2:
			zgui::Checkbox("skeleton", &Settings::Visuals::SkeletonESP);
			zgui::Checkbox("box", &Settings::Visuals::BoxESP);
			zgui::Checkbox("line", &Settings::Visuals::LineESP);
			zgui::Checkbox("name", &Settings::Visuals::NameESP);
			zgui::SliderFloat("esp distance", 5.f, 500.f, &Settings::Visuals::ESPDistance);
			break;
		case 3:
			zgui::Checkbox("fov changer", &Settings::Exploits::FOVChanger);
			zgui::SliderFloat("fov value", 5.f, 160.f, &Settings::Exploits::FOVValue);
			break;
		}
		zgui::EndWindow();
	}
}