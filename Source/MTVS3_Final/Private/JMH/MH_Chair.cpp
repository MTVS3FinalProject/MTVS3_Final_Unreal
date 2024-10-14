// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Chair.h"

#include "AssetTypeCategories.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AMH_Chair::AMH_Chair()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Boxcomp=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(Boxcomp);

	Boxcomp->OnComponentBeginOverlap.AddDynamic(this , &AMH_Chair::OnBeginOverlap);
	Boxcomp->OnComponentEndOverlap.AddDynamic(this , &AMH_Chair::OnEndOverlap);

	Widgetcomp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	Widgetcomp->SetupAttachment(Boxcomp);

}

// Called when the game starts or when spawned
void AMH_Chair::BeginPlay()
{
	Super::BeginPlay();
	Widgetcomp->SetVisibility(false);

	APlayerController* Pc = GetWorld()->GetFirstPlayerController();
	if(Pc)
	{
		Pc->SetInputMode(FInputModeUIOnly());
	}
}

// Called every frame
void AMH_Chair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMH_Chair::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp ,
                               int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player&&Player->IsLocallyControlled())
	{
		ShowText();
	}
}

void AMH_Chair::OnEndOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                             UPrimitiveComponent* OtherComp , int32 OtherBodyIndex)
{
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player&&Player->IsLocallyControlled())
	{
		HideText();
	}
}

void AMH_Chair::ShowText()
{
	Widgetcomp->SetVisibility(true);
}

void AMH_Chair::HideText()
{
	Widgetcomp->SetVisibility(false);
}
