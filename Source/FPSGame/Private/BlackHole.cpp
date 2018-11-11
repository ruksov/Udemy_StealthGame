// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABlackHole::ABlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComp;

    InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
    InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::OnOverlapInnerSphere);
    InnerSphereComp->SetupAttachment(MeshComp);

    OuterSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphereComp"));
    OuterSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OuterSphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    OuterSphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
    OuterSphereComp->SetupAttachment(MeshComp);
}

// Called when the game starts or when spawned
void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlackHole::OnOverlapInnerSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        OtherActor->Destroy();
    }
}

// Called every frame
void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    /// Get overlapping components
    TArray<UPrimitiveComponent*> OverlappingComponents;
    OuterSphereComp->GetOverlappingComponents(OverlappingComponents);

    /// Add radial force for each overlapping components
    const float OuterSphereRadius = OuterSphereComp->GetScaledSphereRadius();
    for (auto Comp : OverlappingComponents)
    {
        if (Comp && Comp->IsSimulatingPhysics())
        {
            Comp->AddRadialForce(GetActorLocation()
                , OuterSphereRadius
                , ForceStrength
                , ERadialImpulseFalloff::RIF_Constant
                , true);
        }
    }
}

