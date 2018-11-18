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

    void SetGuardState(EAIGuardState NewState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnPawnSeen(APawn* SeenPawn);

    UFUNCTION()
    void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

    UFUNCTION(BlueprintImplementableEvent, Category = "AI")
    void OnGuardStateChanged(EAIGuardState NewState);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UPawnSensingComponent* PawnSensingComp;

private:
    FRotator OriginalRotation;
    FTimerHandle TimerHandle_ResetRotation;
    EAIGuardState CurrentState = EAIGuardState::Idle;
};
