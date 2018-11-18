// Fill out your copyright notice in the Description page of Project Settings.

#include "LaunchPad.h"

#include "FPSCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALaunchPad::ALaunchPad()
{
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComp;

    OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
    OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ALaunchPad::HandleOverlap);
    OverlapComp->SetupAttachment(RootComponent);

    ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
    ArrowComp->SetupAttachment(RootComponent);
}

void ALaunchPad::HandleOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (!OtherActor)
    {
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Overlapped by actor!"));
    auto LaunchVelocity = ArrowComp->GetComponentRotation().Vector() * LaunchStrength;

    auto MyPawn = Cast<AFPSCharacter>(OtherActor);
    if (MyPawn)
    {
        MyPawn->LaunchCharacter(LaunchVelocity, true, true);

        // spawn FX 
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
            ActivateLaunchPadEffect,
            GetActorLocation());
    }
    else if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

        // spawn FX 
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
            ActivateLaunchPadEffect,
            GetActorLocation());
    }
}
