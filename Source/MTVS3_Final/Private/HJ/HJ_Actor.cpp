// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HJ_Actor.h"
#include "HJ/TTPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "HJ/TTPlayer.h"
#include "JMH/MH_Interaction.h"
#include "JMH/MainWidget.h"

// Sets default values
AHJ_Actor::AHJ_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Boxcomp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(Boxcomp);

	Boxcomp->OnComponentBeginOverlap.AddDynamic(this , &AHJ_Actor::OnBeginOverlap);
	Boxcomp->OnComponentEndOverlap.AddDynamic(this , &AHJ_Actor::OnEndOverlap);

	Widgetcomp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	Widgetcomp->SetupAttachment(Boxcomp);
}

// Called when the game starts or when spawned
void AHJ_Actor::BeginPlay()
{
	Super::BeginPlay();
	Widgetcomp->SetVisibility(false);
}

// Called every frame
void AHJ_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHJ_Actor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if ( TTPlayer && TTPlayer->IsLocallyControlled() )
	{
		SetMainUI(TTPlayer->MainUI);

		OverlappingPlayer = TTPlayer;  // 오버랩된 플레이어 추적
		ShowText();
	}
}

void AHJ_Actor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if ( TTPlayer && TTPlayer->IsLocallyControlled() )
	{
		OverlappingPlayer = nullptr;  // 오버랩 해제 시 플레이어 초기화
		HideText();
		if ( MainUI ) MainUI->SetWidgetSwitcher(0);
	}
}

void AHJ_Actor::ShowText()
{
	Widgetcomp->SetVisibility(true);
	// GetWidget()을 사용하여 위젯 인스턴스를 가져옴
	UUserWidget* WidgetCompUI = Cast<UUserWidget>(Widgetcomp->GetWidget());
	if ( WidgetCompUI )
	{
		// 위젯 인스턴스를 UMH_Interaction으로 캐스팅
		UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(WidgetCompUI);
		if ( InteractionUI )
		{
			InteractionUI->SetActiveWidgetIndex(1);
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("No interaction UI: %s") , *GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("No widget found in component: %s") , *GetName());
	}
}

void AHJ_Actor::HideText()
{
	Widgetcomp->SetVisibility(false);
}

void AHJ_Actor::SetMainUI(UMainWidget* InMainUI)
{
	MainUI = InMainUI;
}

