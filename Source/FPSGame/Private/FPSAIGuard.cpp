// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"

#include "FPSGameMode.h"

#include "Perception/PawnSensingComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

void AFPSAIGuard::SetGuardState(EAIGuardState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    OnGuardStateChanged(NewState);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
    OriginalRotation = GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn * SeenPawn)
{
    if (!SeenPawn)
    {
        return;
    }

    DrawDebugSphere(GetWorld()
        , SeenPawn->GetActorLocation()
        , 32.f
        , 12
        , FColor::Green
        , false
        , 10.f);

    auto World = GetWorld();
    auto GM = World ? Cast<AFPSGameMode>(World->GetAuthGameMode()) : nullptr;
    if (GM)
    {
        GM->CompleteMission(SeenPawn, false);
    }

    SetGuardState(EAIGuardState::Alerted);
}

void AFPSAIGuard::OnNoiseHeard(APawn * NoiseInstigator, const FVector & Location, float Volume)
{
    if (CurrentState == EAIGuardState::Alerted)
    {
        return;
    }

    DrawDebugSphere(GetWorld()
        , Location
        , 32.f
        , 12
        , FColor::Blue
        , false
        , 10.f);

    auto Direction = Location - GetActorLocation();
    Direction.Normalize();
    
    auto NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
    NewLookAt.Pitch = 0;
    NewLookAt.Roll = 0;
    SetActorRotation(NewLookAt);

    auto& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.ClearTimer(TimerHandle_ResetRotation);

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindLambda(
        [this]() 
    { 
        if (CurrentState == EAIGuardState::Alerted)
        {
            return;
        }

        SetActorRotation(OriginalRotation); 
        SetGuardState(EAIGuardState::Idle);
    });

    TimerManager.SetTimer(TimerHandle_ResetRotation
        , TimerDelegate
        , 3.0f
        , false);

    SetGuardState(EAIGuardState::Suspicious);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
