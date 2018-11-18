// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

UCLASS()
class FPSGAME_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UBoxComponent* OverlapComp;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UArrowComponent* ArrowComp;

    UPROPERTY(EditDefaultsOnly, Category = "LaunchPad")
    class UParticleSystem* ActivateLaunchPadEffect;

    UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
    float LaunchStrength = 1000.f;

protected:
    UFUNCTION()
        void HandleOverlap(UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult & SweepResult);
};
