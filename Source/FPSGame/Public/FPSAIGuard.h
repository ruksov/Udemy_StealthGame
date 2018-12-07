// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

UENUM(BlueprintType)
enum class EAIGuardState : uint8
{
    Idle,
    Suspicious,
    Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnPawnSeen(APawn* SeenPawn);

    UFUNCTION()
    void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
    void OnGuardStateChanged(EAIGuardState NewState);

    UFUNCTION()
    void OnRep_GuardState();

    void SetGuardState(EAIGuardState NewState);

    void ResetOrientation();

    void StopMovement();
    void MoveToNextPatrolPoint();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UPawnSensingComponent* PawnSensingComp;

    // Let the guard go on patrol
    UPROPERTY(EditInstanceOnly, Category = "AI")
    bool bPatrol = false;

    // Array of patrol points
    UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition="bPatrol"))
    TArray<AActor*> PatrolPoints;

    // The current point the actor is either moving to or standing at
    size_t CurrentPatrolIndex = 0;

    UPROPERTY(ReplicatedUsing=OnRep_GuardState)
    EAIGuardState CurrentState = EAIGuardState::Idle;

private:
    FRotator OriginalRotation;
    FTimerHandle TimerHandle_ResetRotation;
};
