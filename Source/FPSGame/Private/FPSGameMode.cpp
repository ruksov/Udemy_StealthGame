// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"

#include <UObject/ConstructorHelpers.h>
#include <Kismet/GameplayStatics.h>
#include "Engine/World.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionComplete)
{
    if (InstigatorPawn)
    {
        if (SpectatingViewpointClass)
        {
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, FoundActors);

            AActor* NewViewpointActor = nullptr;
            if (FoundActors.Num() > 0)
            {
                NewViewpointActor = FoundActors[0];

                for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
                {
                    auto PC = Cast<APlayerController>(It->Get());
                    if (PC && NewViewpointActor)
                    {
                        PC->SetViewTargetWithBlend(NewViewpointActor, 0.7f, EViewTargetBlendFunction::VTBlend_Cubic);
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpointClass is nullptr. Please update GameMode class with valid subclass. Cannot change spectating viewpoint."));
        }
    }

    auto GS = GetGameState<AFPSGameState>();
    if (GS)
    {
        GS->MulticastOnMissionComplete(InstigatorPawn, bMissionComplete);
    }
}
