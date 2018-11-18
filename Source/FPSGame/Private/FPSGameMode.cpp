// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"

#include <UObject/ConstructorHelpers.h>
#include <Kismet/GameplayStatics.h>

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
        InstigatorPawn->DisableInput(nullptr);

        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, FoundActors);

        AActor* NewViewpointActor = nullptr;
        if (FoundActors.Num() > 0)
        {
            NewViewpointActor = FoundActors[0];
        }

        auto PC = Cast<APlayerController>(InstigatorPawn->GetController());
        if (PC && NewViewpointActor)
        {
            PC->SetViewTargetWithBlend(NewViewpointActor, 0.7f, EViewTargetBlendFunction::VTBlend_Cubic);
        }
    }

    OnMissionCompleted(InstigatorPawn, bMissionComplete);
}
