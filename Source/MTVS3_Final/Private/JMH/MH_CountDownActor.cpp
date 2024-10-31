// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_CountDownActor.h"
#include "Components/WidgetComponent.h"
#include "JMH/MH_GameStartCountDown.h"

// Sets default values
AMH_CountDownActor::AMH_CountDownActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMH_CountDownActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

// Called every frame
void AMH_CountDownActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMH_CountDownActor::MulticastStartCountDownVisible_Implementation(bool visible)
{
	SetActorHiddenInGame(false);
	UUserWidget* WidgetCompUI = Cast<UUserWidget>(WidgetComp->GetWidget());
	UMH_GameStartCountDown* CountDownUI = Cast<UMH_GameStartCountDown>(WidgetCompUI);
	if (WidgetCompUI)
	{
		if (visible)
		{
			WidgetComp->SetVisibility(true);
			CountDownUI->StartCountdown();
		}
		else
		{
			this->Destroy();
		}
	}
}
