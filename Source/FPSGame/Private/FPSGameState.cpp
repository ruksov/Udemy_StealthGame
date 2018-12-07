// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"

#include "Engine/World.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionComplete)
{
    for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        auto PC = Cast<AFPSPlayerController>(It->Get());
        if (PC && PC->IsLocalController())
        {
            // Send mission complete event to blueprint
            PC->OnMissionCompleted(InstigatorPawn, bMissionComplete);

            /// Disable input
            auto Pawn = PC->GetPawn();
            if (Pawn)
            {
                Pawn->DisableInput(PC);
            }
        }
    }
}
