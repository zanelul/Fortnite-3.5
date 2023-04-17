#include "../SDK/SDK.hpp"
#include <Windows.h>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <numbers>

namespace zgui {
	namespace globals {
		namespace other {
			inline wchar_t* s2wc(const char* c) {
				const size_t cSize = strlen(c) + 1;
				wchar_t* wc = new wchar_t[cSize];
				mbstowcs(wc, c, cSize);

				return wc;
			}

			inline uint32_t Hash(const char* Str, uint32_t Value = 0x811c9dc5) {
				return *Str ? Hash(Str + 1, (Value ^ *Str) * 0x1000193ull) : Value;
			}

			struct Render_t {
				int Type;
				SDK::FVector2D Pos1;
				SDK::FVector2D Pos2;
				SDK::FLinearColor Color;
				std::string Title = "";
				bool Center = false;
			};
		}

		inline SDK::UCanvas* Canvas;
		inline SDK::UFont* Font;
		inline bool KeyState[256];
		inline bool PrevKeyState[256];
		inline SDK::FVector2D MousePos;
		inline SDK::FVector2D PrevMousePos;
		inline bool IsMenuOpen = true;
		inline bool IsMenuDragging = true;
		inline std::vector<other::Render_t> Renders;
		inline SDK::FVector2D WndPos{100.f, 100.f};
		inline SDK::FVector2D BasePos{ 16, 23 };
		inline int ItemSpacing = 10;
		inline std::stack<SDK::FVector2D> CursorPos;
		inline uint32_t Blocking = 0;

		/* Colors */
		inline SDK::FLinearColor BorderInnerFill(0.0414518916115f, 0.0414518916115f, 0.0414518916115f, 1.f);
		inline SDK::FLinearColor BorderFill(0.0169880520893f, 0.0169880520893f, 0.0169880520893f, 1.f);
		inline SDK::FLinearColor BorderColor(0.000804658499513f, 0.000804658499513f, 0.000804658499513f, 1.f);
		inline SDK::FLinearColor WindowBg(0.00902149189801, 0.00902149189801, 0.00902149189801, 1.f);
		inline SDK::FLinearColor TextColor(0.605484299910907f, 0.605484299910907f, 0.605484299910907f, 1.f);
		inline SDK::FLinearColor TextDimmerColor(0.124740945387f, 0.15728072789f, 0.170138373223f, 1.f);
		inline SDK::FLinearColor ControlOutline(0.00502820345686f, 0.00502820345686f, 0.00902149189801f, 1.f);
		inline SDK::FLinearColor ControlActive(0.151058046871f, 0.106156067813f, 0.804559113895f, 1.f);
		inline SDK::FLinearColor ControlIdle(0.0445526273164f, 0.0445526273164f, 0.0619074756055f, 1.f);
		inline SDK::FLinearColor ControlSlider(0.151058046871f, 0.106156067813f, 0.804559113895f, 1.f);
	}

	namespace drawlist {
		namespace position {
			inline void PushCursorPos(SDK::FVector2D NewPos) {
				if (!globals::IsMenuOpen)
					return;

				globals::CursorPos.push(NewPos);
			}

			inline SDK::FVector2D PopCursorPos() {
				if (!globals::IsMenuOpen)
					return {};

				SDK::FVector2D Pos = globals::CursorPos.top();
				globals::CursorPos.pop();
				return Pos;
			}
		}
		
		inline void SetCanvas(SDK::UCanvas* NewCanvas) {
			globals::Canvas = NewCanvas;
		}

		inline void SetFont(SDK::UFont* NewFont) {
			globals::Font = NewFont;
		}
		
		inline void AddFilledRect(SDK::FVector2D Pos1, SDK::FVector2D Pos2, SDK::FLinearColor Color) {
			for (int i = 0; i < Pos2.Y; i++) {
				globals::Canvas->K2_DrawLine(SDK::FVector2D(Pos1.X, Pos1.Y + i), SDK::FVector2D(Pos1.X + Pos2.X, Pos1.Y + i), 1.f, Color);
			}
		}

		inline void AddRect(SDK::FVector2D Pos1, SDK::FVector2D Pos2, SDK::FLinearColor Color) {
			globals::Canvas->K2_DrawBox(Pos1, Pos2, 1.f, Color);
		}

		inline void AddLine(SDK::FVector2D Pos1, SDK::FVector2D Pos2, SDK::FLinearColor Color) {
			globals::Canvas->K2_DrawLine(Pos1, Pos2, 1.f, Color);
		}

		inline void AddText(SDK::FVector2D Pos, std::string Text, SDK::FLinearColor Color, bool Center = false) {
			globals::Canvas->K2_DrawText(globals::Font, SDK::FString(globals::other::s2wc(Text.c_str())), Pos, Color, 1.f, SDK::FLinearColor(), SDK::FVector2D(), Center, Center, true, SDK::FLinearColor(0.f, 0.f, 0.f, 1.f));
		}

		inline void AddCircle(SDK::FVector2D Pos, float Radius, int NumSides, SDK::FLinearColor Color) {
			float Step = std::numbers::pi * 2.0 / NumSides;
			int Count = 0;
			for (float i = 0; i < std::numbers::pi * 2.0; i += Step) {
				float X1 = Radius * cos(i) + Pos.X;
				float Y1 = Radius * sin(i) + Pos.Y;
				float X2 = Radius * cos(i + Step) + Pos.X;
				float Y2 = Radius * sin(i + Step) + Pos.Y;
				AddLine(SDK::FVector2D(X1, Y1), SDK::FVector2D(X2, Y2), Color);
			}
			/*double Angle = 2 * std::numbers::pi / NumSides;
			for (int i = 0; i < NumSides; i++) {
				float X = Pos.X + Radius * cos(Angle * i);
				float Y = Pos.Y + Radius * sin(Angle * i);
				float NextX = Pos.X + Radius * cos(Angle * (i + 1));
				float NextY = Pos.X + Radius * sin(Angle * (i + 1));

				AddLine(SDK::FVector2D(X, Y), SDK::FVector2D(NextX, NextY), Color);
			}*/
		}

		inline SDK::FVector2D GetTextSize(std::string Text) {
			return globals::Canvas->K2_StrLen(globals::Font, SDK::FString(globals::other::s2wc(Text.c_str())));
		}
	}

	inline void PollInput(std::string WindowName) {
		for (int i = 0; i < 256; i++) {
			globals::PrevKeyState[i] = globals::KeyState[i];
			globals::KeyState[i] = GetAsyncKeyState(i);
		}

		POINT MousePos;
		GetCursorPos(&MousePos);
		ScreenToClient(FindWindowA(nullptr, WindowName.c_str()), &MousePos);
		globals::PrevMousePos = globals::MousePos;
		globals::MousePos = SDK::FVector2D((float)MousePos.x, (float)MousePos.y);
	}

	inline bool KeyPressed(int Key) {
		return globals::KeyState[Key] && !globals::PrevKeyState[Key];
	}

	inline bool KeyDown(int Key) {
		return globals::KeyState[Key];
	}

	inline bool KeyReleased(int Key) {
		return !globals::KeyState[Key] && globals::PrevKeyState[Key];
	}

	inline bool IsMouseInRegion(SDK::FVector2D Pos1, SDK::FVector2D Pos2) {
		return globals::MousePos.X > Pos1.X && globals::MousePos.Y > Pos1.Y && globals::MousePos.X < Pos2.X + Pos1.X && globals::MousePos.Y < Pos2.Y + Pos1.Y;
	}

	inline void SameLine(float X = -1) {
		SDK::FVector2D CursorPos = drawlist::position::PopCursorPos();

		if (X != -1)
			drawlist::position::PushCursorPos(SDK::FVector2D(globals::BasePos.X + X, CursorPos.Y));
	}

	inline bool BeginWindow(std::string Title, SDK::FVector2D Size, int MenuKey) {
		if (KeyPressed(MenuKey))
			globals::IsMenuOpen = !globals::IsMenuOpen;

		if (globals::IsMenuOpen) {
			if (IsMouseInRegion(SDK::FVector2D(globals::WndPos.X - 6, globals::WndPos.Y - 10), SDK::FVector2D(Size.X + 12, Size.Y + 16)) && KeyPressed(VK_LBUTTON) && !globals::IsMenuDragging) {
				globals::IsMenuDragging = true;
			}
			else if (KeyDown(VK_LBUTTON) && globals::IsMenuDragging) {
				SDK::FVector2D MouseDelta = SDK::FVector2D(globals::MousePos.X - globals::PrevMousePos.X, globals::MousePos.Y - globals::PrevMousePos.Y);
				SDK::FVector2D NewPos = SDK::FVector2D(globals::WndPos.X + MouseDelta.X, globals::WndPos.Y + MouseDelta.Y);
				globals::WndPos = NewPos;
			}
			else if (!KeyDown(VK_LBUTTON) && globals::IsMenuDragging)
				globals::IsMenuDragging = false;

			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X - 6, globals::WndPos.Y - 10), SDK::FVector2D(Size.X + 12, Size.Y + 16), globals::BorderInnerFill);
			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X - 5, globals::WndPos.Y - 9), SDK::FVector2D(Size.X + 10, Size.Y + 14), globals::BorderColor);
			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X - 4, globals::WndPos.Y - 8), SDK::FVector2D(Size.X + 8, Size.Y + 12), globals::BorderFill);
			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X, globals::WndPos.Y + 7), SDK::FVector2D(Size.X, Size.Y - 7), globals::BorderColor);
			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X + 1, globals::WndPos.Y + 8), SDK::FVector2D(Size.X - 2, Size.Y - 9), globals::BorderInnerFill);
			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X + 8, globals::WndPos.Y + 15), SDK::FVector2D(Size.X - 16, Size.Y - 23), globals::BorderColor);

			drawlist::AddText(SDK::FVector2D(globals::WndPos.X + Size.X * 0.5f, globals::WndPos.Y + (Size.Y * 0.010f) - 5.f), Title, globals::TextColor, true);

			drawlist::AddFilledRect(SDK::FVector2D(globals::WndPos.X + 9, globals::WndPos.Y + 16), SDK::FVector2D(Size.X - 18, Size.Y - 25), globals::WindowBg);

			drawlist::position::PushCursorPos(globals::BasePos);
		}

		return globals::IsMenuOpen;
	}

	inline void EndWindow() {
		if (!globals::IsMenuOpen)
			return;

		for (int i = 0; i < globals::Renders.size(); i++) {
			auto Render = globals::Renders[i];

			switch (Render.Type) {
			case 0: // Rect Filled
				drawlist::AddFilledRect(Render.Pos1, Render.Pos2, Render.Color);
				break;
			case 2: // Text
				drawlist::AddText(Render.Pos1, Render.Title, Render.Color, Render.Center);
				break;
			}
		}

		globals::Renders.clear();

		while (!globals::CursorPos.empty())
			globals::CursorPos.pop();
	}

	inline bool Button(std::string Title, SDK::FVector2D Size) {
		SDK::FVector2D CursorPos = drawlist::position::PopCursorPos();
		SDK::FVector2D DrawPos = SDK::FVector2D(globals::WndPos.X + CursorPos.X, globals::WndPos.Y + CursorPos.Y);

		bool Result = false;
		bool Active = globals::Blocking == globals::other::Hash(Title.c_str());

		if (bool Hovered = IsMouseInRegion(DrawPos, Size); !Active && Hovered && KeyPressed(VK_LBUTTON)) {
			globals::Blocking = globals::other::Hash(Title.c_str());
		}
		else if (Active && !KeyDown(VK_LBUTTON)) {
			globals::Blocking = 0;
			Result = Hovered;
		}

		SDK::FVector2D TextSize = drawlist::GetTextSize(Title);

		globals::Renders.emplace_back(globals::other::Render_t{ 0, DrawPos, Size, globals::ControlOutline });
		globals::Renders.emplace_back(globals::other::Render_t{ 0, SDK::FVector2D(DrawPos.X + 1, DrawPos.Y + 1), SDK::FVector2D(Size.X - 2, Size.Y - 2), Active ? globals::ControlActive : globals::ControlIdle });
		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X + ((Size.X - TextSize.X) / 2) + TextSize.X / 2, DrawPos.Y + ((Size.Y - TextSize.Y) / 2) + TextSize.Y / 2), SDK::FVector2D(), globals::TextColor, Title, true });

		drawlist::position::PushCursorPos(SDK::FVector2D(CursorPos.X + Size.X + globals::ItemSpacing, CursorPos.Y));
		drawlist::position::PushCursorPos(SDK::FVector2D(CursorPos.X, CursorPos.Y + Size.Y + globals::ItemSpacing));
		return Result;
	}

	inline void Checkbox(std::string Title, bool* Value) {
		SDK::FVector2D CursorPos = drawlist::position::PopCursorPos();
		SDK::FVector2D DrawPos = SDK::FVector2D(globals::WndPos.X + CursorPos.X, globals::WndPos.Y + CursorPos.Y);
		const int ControlHeight = 10;
		const int ControlWidth = 10;
		SDK::FVector2D TextSize = drawlist::GetTextSize(Title);
		bool Active = globals::Blocking == globals::other::Hash(Title.c_str());

		if (bool Hovered = IsMouseInRegion(DrawPos, SDK::FVector2D(ControlWidth + 6 + TextSize.X, ControlHeight)); !Active && Hovered && KeyPressed(VK_LBUTTON)) {
			globals::Blocking = globals::other::Hash(Title.c_str());
		}
		else if (Active && !KeyDown(VK_LBUTTON)) {
			globals::Blocking = 0;
			*Value = !*Value;
		}

		globals::Renders.emplace_back(globals::other::Render_t{ 0, DrawPos, SDK::FVector2D(ControlWidth, ControlHeight), globals::ControlOutline });
		globals::Renders.emplace_back(globals::other::Render_t{ 0, SDK::FVector2D(DrawPos.X + 1, DrawPos.Y + 1), SDK::FVector2D(ControlWidth - 2, ControlHeight - 2), *Value ? globals::ControlActive : globals::ControlIdle });
		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X + 14, DrawPos.Y - 4), SDK::FVector2D(), *Value ? globals::TextColor : globals::TextDimmerColor, Title });

		drawlist::position::PushCursorPos(SDK::FVector2D(CursorPos.X + 14 + TextSize.X, CursorPos.Y));
		drawlist::position::PushCursorPos(SDK::FVector2D(CursorPos.X, CursorPos.Y + ControlWidth + globals::ItemSpacing));
	}

	inline void Text(std::string Title) {

	}

	inline void SliderFloat(std::string Title, float Min, float Max, float* Value) {
		SDK::FVector2D CursorPos = drawlist::position::PopCursorPos();
		SDK::FVector2D DrawPos = SDK::FVector2D(globals::WndPos.X + CursorPos.X, globals::WndPos.Y + CursorPos.Y);
		const int ControlWidth = 120;
		const int ControlHeight = 10;

		SDK::FVector2D TextSize = drawlist::GetTextSize(Title);
		DrawPos.Y += TextSize.Y;

		if (globals::Blocking == 0 && IsMouseInRegion(SDK::FVector2D(DrawPos.X - (ControlHeight - 2), DrawPos.Y), SDK::FVector2D(8, 10)) && KeyPressed(VK_LBUTTON))
			*Value = std::clamp(*Value - 1, Min, Max);
		else if (globals::Blocking == 0 && IsMouseInRegion(SDK::FVector2D(DrawPos.X + ControlWidth, DrawPos.Y), SDK::FVector2D(8, 10)) && KeyPressed(VK_LBUTTON))
			*Value = std::clamp(*Value + 1, Min, Max);

		if (bool Hovered = IsMouseInRegion(DrawPos, SDK::FVector2D(ControlWidth, ControlHeight)); Hovered && KeyPressed(VK_LBUTTON) && globals::Blocking == 0)
			globals::Blocking = globals::other::Hash(Title.c_str());
		else if (KeyDown(VK_LBUTTON) && globals::Blocking == globals::other::Hash(Title.c_str())) {
			float ValueUnMapped = std::clamp(globals::MousePos.X - DrawPos.X, 0.f, (float)ControlWidth);
			float ValueMapped = (ValueUnMapped / ControlWidth) * (Max - Min) + Min;
			*Value = ValueMapped;
		}
		else if (!KeyDown(VK_LBUTTON) && globals::Blocking == globals::other::Hash(Title.c_str()))
			globals::Blocking = 0;

		float DynamicWidth = (*Value - Min) / (Max - Min) * ControlWidth - 2;

		std::stringstream ss;
		ss << std::fixed << std::setprecision(1) << *Value;
		std::string ValueStr = ss.str();
		SDK::FVector2D ValueTextSize = drawlist::GetTextSize(ValueStr);
		float ValueTextX = DynamicWidth - ValueTextSize.X;

		globals::Renders.emplace_back(globals::other::Render_t{ 0, DrawPos, SDK::FVector2D(ControlWidth, ControlHeight), globals::ControlOutline });
		globals::Renders.emplace_back(globals::other::Render_t{ 0, SDK::FVector2D(DrawPos.X + 1, DrawPos.Y + 1), SDK::FVector2D(ControlWidth - 2, ControlHeight - 2), globals::ControlIdle });
		globals::Renders.emplace_back(globals::other::Render_t{ 0, SDK::FVector2D(DrawPos.X + 1, DrawPos.Y + 1), SDK::FVector2D(DynamicWidth, ControlHeight - 2), globals::ControlSlider });

		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X - (ControlHeight - 2), DrawPos.Y - 6), SDK::FVector2D(), globals::TextDimmerColor, "-"});
		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X + (ControlWidth + 4), DrawPos.Y - 6), SDK::FVector2D(), globals::TextDimmerColor, "+"});
		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X + ValueTextX, DrawPos.Y), SDK::FVector2D(), globals::TextColor, ValueStr });
		globals::Renders.emplace_back(globals::other::Render_t{ 2, SDK::FVector2D(DrawPos.X, (DrawPos.Y - TextSize.Y) - 4), SDK::FVector2D(), globals::TextColor, Title });
	}
}