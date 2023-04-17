#include "Cheat.h"
#include "../Menu/zgui.hpp"
#include "../Menu/Settings/Settings.h"
#include <format>

std::vector<std::pair<int, int>> SkeletonPairs = {
    { 66, 64 },     // Head to Neck
    { 64, 5 },      // Neck to Center Hip
    { 5, 2 },       // Center Hip to Hip
    { 2, 74 },      // Hip to Right Hip
    { 2, 67 },      // Hip to Left Hip
    { 74, 75 },     // Right Hip to Right Knee
    { 67, 68 },     // Left Hip to Left Knee
    { 75, 83 },     // Right Knee to Right Foot
    { 68, 82 },     // Left Knee to Left Foot
    { 64, 9 },      // Neck to Left Shoulder
    { 64, 37 },     // Neck to Right Shoulder
    { 37, 38 },     // Right Shoulder to Right Elbow
    { 9, 10 },      // Left Shoulder to Left Elbow
    { 10, 32 },     // Left Elbow to Left Hand
    { 38, 60 },     // Right Elbow to Right Hand
};

constexpr float Deg = 180.0f / std::numbers::pi;
inline SDK::FRotator CalcAngle(SDK::FVector CameraLocation, SDK::FVector TargetPoint) {
    SDK::FVector Rel = SDK::FVector(TargetPoint.X - CameraLocation.X, TargetPoint.Y - CameraLocation.Y, TargetPoint.Z - CameraLocation.Z);
    float Pitch = atan2f(Rel.Z, sqrt((Rel.X * Rel.X) + (Rel.Y * Rel.Y))) * Deg;
    float Yaw = atan2f(Rel.Y, Rel.X) * Deg;
    return SDK::FRotator(Pitch, Yaw, 0.0f);
}

void Cheat::Render() {
    static uintptr_t Base = (uintptr_t)GetModuleHandleA("FortniteClient-Win64-Shipping.exe");
    static float ScreenX = zgui::globals::Canvas->SizeX;
    static float ScreenY = zgui::globals::Canvas->SizeY;
    SDK::UWorld* GWorld = *(SDK::UWorld**)(Base + 0x4C6D920);
    if (!GWorld) return;
    SDK::UGameInstance* GameInstance = GWorld->OwningGameInstance;
    if (!GameInstance) return;
    SDK::ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
    if (!LocalPlayer) return;
    SDK::AFortPlayerController* LocalPlayerController = (SDK::AFortPlayerController*)LocalPlayer->PlayerController;
    if (!LocalPlayerController) return;
    SDK::FVector CameraLocation = LocalPlayerController->PlayerCameraManager->GetCameraLocation();

    if (Settings::Exploits::FOVChanger) {
        LocalPlayerController->FOV(Settings::Exploits::FOVValue);
    }

    if (Settings::Aim::EnableAim) {
        zgui::drawlist::AddCircle(SDK::FVector2D(ScreenX / 2, ScreenY / 2), Settings::Aim::AimRadius, 100, SDK::FLinearColor(1, 1, 1, 1));
    }
    
    auto Levels = GWorld->Levels;

    float ClosestDistance = FLT_MAX;
    SDK::FVector AimLocation{};
    for (int i = 0; i < Levels.Num(); i++) {
        SDK::ULevel* Level = Levels[i];
        if (!Level) continue;
        auto Actors = Level->Actors;
        if (!Actors.IsValid()) continue;
        
        for (int j = 0; j < Actors.Num(); j++) {
            SDK::AActor* Actor = Actors[j];
            if (!Actor) continue;

            if (Actor->IsA(SDK::AFortPlayerPawn::StaticClass())) {
                SDK::APlayerPawn_Athena_C* PlayerPawn = (SDK::APlayerPawn_Athena_C*)Actor;
                if (PlayerPawn->Controller == LocalPlayerController) continue;
                SDK::USkeletalMeshComponent* Mesh = PlayerPawn->Mesh;
                if (!Mesh) continue;
                SDK::APlayerState* PlayerState = PlayerPawn->PlayerState;
                if (!PlayerState) continue;

                /*for (int k = 0; k < Mesh->GetNumBones(); k++) {
                    SDK::FVector2D BonePos;
                    if (!LocalPlayerController->ProjectWorldLocationToScreen(Mesh->GetBoneLocation(k), &BonePos, true)) continue;
                    zgui::drawlist::AddText(BonePos, std::to_string(k), SDK::FLinearColor(1, 1, 1, 1));
                }*/

                SDK::FVector2D RootPos, HeadPos;
                SDK::FVector HeadPos3D = Mesh->GetBoneLocation(66);
                SDK::FVector RootPos3D = Mesh->GetBoneLocation(0);
                if (!LocalPlayerController->ProjectWorldLocationToScreen(RootPos3D, &RootPos, true) ||
                    !LocalPlayerController->ProjectWorldLocationToScreen(SDK::FVector(HeadPos3D.X, HeadPos3D.Y, HeadPos3D.Z + 20.f), &HeadPos, true))
                    continue;

                float PlayerDistance = sqrt(pow(RootPos3D.X - CameraLocation.X, 2) + pow(RootPos3D.Y - CameraLocation.Y, 2) + pow(RootPos3D.Z - CameraLocation.Z, 2)) / 100;
                if (PlayerDistance > Settings::Visuals::ESPDistance) continue;

                if (Settings::Visuals::SkeletonESP) {
                    for (int k = 0; k < SkeletonPairs.size(); k++) {
                        auto Pair = SkeletonPairs[k];

                        SDK::FVector2D BonePos1, BonePos2;
                        if (!LocalPlayerController->ProjectWorldLocationToScreen(Mesh->GetBoneLocation(Pair.first), &BonePos1, true) ||
                            !LocalPlayerController->ProjectWorldLocationToScreen(Mesh->GetBoneLocation(Pair.second), &BonePos2, true))
                            continue;

                        zgui::drawlist::AddLine(BonePos1, BonePos2, SDK::FLinearColor(1, 0, 0, 1));
                    }
                }

                if (Settings::Visuals::BoxESP) {
                    float Height = abs(HeadPos.Y - RootPos.Y);
                    float Width = Height * 0.65;

                    zgui::drawlist::AddRect(SDK::FVector2D(HeadPos.X - (Width / 2), HeadPos.Y), SDK::FVector2D(Width, Height), SDK::FLinearColor(1, 1, 1, 1));
                }

                if (Settings::Visuals::LineESP) {
                    zgui::drawlist::AddLine(SDK::FVector2D(ScreenX / 2, ScreenY), HeadPos, SDK::FLinearColor(1, 0, 0, 1));
                }

                if (Settings::Visuals::NameESP) {
                    zgui::drawlist::AddText(RootPos, "PLAYER", SDK::FLinearColor(1, 1, 1, 1));
                }

                if (Settings::Aim::EnableAim) {
                    float Distance = sqrtf(pow(HeadPos.X - (ScreenX / 2), 2) + pow(HeadPos.Y - (ScreenY / 2), 2));
                    if (Distance > Settings::Aim::AimRadius) continue;

                    if (Distance < ClosestDistance) {
                        ClosestDistance = Distance;
                        AimLocation = HeadPos3D;
                    }
                }
            }
        }
    }

    if (AimLocation.X != 0.f) {
        if (Settings::Aim::AimLine) {
            SDK::FVector2D AimPos;
            if (!LocalPlayerController->ProjectWorldLocationToScreen(AimLocation, &AimPos, true)) return;
            zgui::drawlist::AddLine(SDK::FVector2D(ScreenX / 2, ScreenY / 2), AimPos, SDK::FLinearColor(0, 1, 0, 1));
        }

        if (GetAsyncKeyState(VK_RBUTTON)) {
            SDK::FRotator Angle = CalcAngle(CameraLocation, AimLocation);
            LocalPlayerController->SetControlRotation(Angle);
        }
    }
}