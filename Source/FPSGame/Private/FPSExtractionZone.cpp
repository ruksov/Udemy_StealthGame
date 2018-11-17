// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSExtractionZone.h"

#include "FPSCharacter.h"
#include "FPSGameMode.h"

#include <Engine/World.h>
#include <Components/BoxComponent.h>
#include <Components/DecalComponent.h>

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
    OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
    OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    OverlapComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    OverlapComp->SetBoxExtent(FVector(200.f), true);
    RootComponent = OverlapComp;

    OverlapComp->SetHiddenInGame(false);

    OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

    DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
    DecalComp->DecalSize = OverlapComp->GetUnscaledBoxExtent();
    DecalComp->SetupAttachment(RootComponent);
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent * OverlappedComponent, 
    AActor * OtherActor, 
    UPrimitiveComponent * OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult & SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("Ovarlapped with extraction zone."));

    auto MyPawn = Cast<AFPSCharacter>(OtherActor);
    if (MyPawn && MyPawn->bIsCarryingObjective)
    {
        auto World = GetWorld();
        auto GM = World ? Cast<AFPSGameMode>(World->GetAuthGameMode()) : nullptr;
        if (GM)
        {
            GM->CompleteMission(MyPawn);
        }
    }
}
