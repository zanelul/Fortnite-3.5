#include "SDK/SDK.hpp"
#include "Hooks/Hooks.h"
#include "Menu/Menu.h"
#include "Cheat/Cheat.h"

using PostRender_fn = void(__thiscall*)(SDK::UGameViewportClient*, SDK::UCanvas*);
PostRender_fn PostRender_o = nullptr;

void PostRender_hk(SDK::UGameViewportClient* This, SDK::UCanvas* Canvas) {
    PostRender_o(This, Canvas);
    Menu::Render(Canvas);
    Cheat::Render();
}

void DllEntry() {
    SDK::Init();

    zgui::drawlist::SetFont(SDK::UObject::FindObject<SDK::UFont>("Font Roboto.Roboto"));

    uintptr_t Base = (uintptr_t)GetModuleHandleA("FortniteClient-Win64-Shipping.exe");
    SDK::UWorld* GWorld = *(SDK::UWorld**)(Base + 0x4C6D920);
    SDK::UGameInstance* GameInstance = GWorld->OwningGameInstance;
    SDK::ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
    SDK::UGameViewportClient* Viewport = LocalPlayer->ViewportClient;

    Hooks::VftTableSwap((PVOID)Viewport, 0x5B, PostRender_hk, (PVOID*)&PostRender_o);
}

BOOL APIENTRY DllMain(HMODULE, DWORD Reason, LPVOID) {
    if (Reason != DLL_PROCESS_ATTACH)
        return FALSE;

    DllEntry();
    return TRUE;
}