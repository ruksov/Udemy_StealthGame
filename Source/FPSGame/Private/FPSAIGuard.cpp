// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"

#include "FPSGameMode.h"

#include "Perception/PawnSensingComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

void AFPSAIGuard::OnRep_GuardState()
{
    OnGuardStateChanged(CurrentState);
}

void AFPSAIGuard::SetGuardState(EAIGuardState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    OnRep_GuardState();
}


void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSAIGuard, CurrentState);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
    OriginalRotation = GetActorRotation();

    if (bPatrol)
    {
        CurrentPatrolIndex = PatrolPoints.Num();
        MoveToNextPatrolPoint();
    }
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

    // Stop movement if patrolling
    StopMovement();
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

    TimerManager.SetTimer(TimerHandle_ResetRotation
        , this
        , &AFPSAIGuard::ResetOrientation
        , 3.0f
        , false);

    SetGuardState(EAIGuardState::Suspicious);

    // Stop movement if patrolling
    StopMovement();
}



void AFPSAIGuard::ResetOrientation()
{
    if (CurrentState == EAIGuardState::Alerted)
    {
        return;
    }

    SetActorRotation(OriginalRotation);
    SetGuardState(EAIGuardState::Idle);

    if (bPatrol)
    {
        MoveToNextPatrolPoint();
    }
}

void AFPSAIGuard::StopMovement()
{
    auto AIController = GetController();
    if (AIController)
    {
        AIController->StopMovement();
    }
}

void AFPSAIGuard::MoveToNextPatrolPoint()
{
    if (PatrolPoints.Num() == 0)
    {
        return;
    }

    if (CurrentPatrolIndex < PatrolPoints.Num() - 1)
    {
        ++CurrentPatrolIndex;
    }
    else
    {
        CurrentPatrolIndex = 0;
    }

    UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), PatrolPoints[CurrentPatrolIndex]);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bPatrol && PatrolPoints.Num() != 0)
    {
        auto Delta = GetActorLocation() - PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
        float DistanceToGoal = Delta.Size2D();

        // Check if we are within 50 units of our goal, if so - pick a new patrol point
        if (DistanceToGoal < 50)
        {
            MoveToNextPatrolPoint();
        }
    }
}
