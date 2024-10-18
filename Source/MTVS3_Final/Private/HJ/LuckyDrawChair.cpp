// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawChair.h"
#include "Components/BoxComponent.h"

// Sets default values
ALuckyDrawChair::ALuckyDrawChair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALuckyDrawChair::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALuckyDrawChair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

