// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawTable.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ALuckyDrawTable::ALuckyDrawTable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);
	
	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComp"));
	TextRenderComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALuckyDrawTable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALuckyDrawTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALuckyDrawTable::MulticastSetTextRender_Implementation(const FText& _RandomSeatNumber)
{
	TextRenderComp->SetText(_RandomSeatNumber);
}
