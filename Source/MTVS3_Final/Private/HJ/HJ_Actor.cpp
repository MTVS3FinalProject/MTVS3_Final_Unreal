// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HJ_Actor.h"
#include "HJ/TTPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_Interaction.h"
#include "JMH/MainWidget.h"

// Sets default values
AHJ_Actor::AHJ_Actor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

	BoxComp->OnComponentBeginOverlap.AddDynamic(this , &AHJ_Actor::OnBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this , &AHJ_Actor::OnEndOverlap);

	// WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	// WidgetComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHJ_Actor::BeginPlay()
{
	Super::BeginPlay();
	// WidgetComp->SetVisibility(false);
}

// Called every frame
void AHJ_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHJ_Actor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                               const FHitResult& SweepResult)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if (TTPlayer && TTPlayer->IsLocallyControlled())
	{
		SetMainUI(TTPlayer->MainUI);

		OverlappingPlayer = TTPlayer; // 오버랩된 플레이어 추적
		ShowText();
	}
}

void AHJ_Actor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                             UPrimitiveComponent* OtherComp , int32 OtherBodyIndex)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if (TTPlayer && TTPlayer->IsLocallyControlled())
	{
		OverlappingPlayer = nullptr; // 오버랩 해제 시 플레이어 초기화
		HideText();
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		if (!GI || !MainUI) return;
		if (GI->GetbIsNewPlayer() == false) MainUI->SetWidgetSwitcher(0);
		else MainUI->SetWidgetSwitcher(10);
	}
}

void AHJ_Actor::ShowText()
{
	// MainUI 체크
	if (!IsValid(MainUI))
	{
		UE_LOG(LogTemp , Warning , TEXT("MainUI is not valid in ShowText"));
		return;
	}
	//if (MainUI) MainUI->SetVisibleInteractionCan(true);

	// WBP_InteractionUI 체크
	if (!IsValid(MainUI->WBP_InteractionUI))
	{
		UE_LOG(LogTemp , Warning , TEXT("WBP_InteractionUI is not valid in ShowText"));
		return;
	}

	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(MainUI->WBP_InteractionUI);
	if (!IsValid(InteractionUI))
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to cast to UMH_Interaction in ShowText"));
		return;
	}

	if (InteractionUI)
	{
		if (ActorHasTag("SelectConcert"))
		{
			InteractionUI->SetActiveWidgetIndex(1);
			if (!bIsInteractionUI)
			{
				bIsInteractionUI = true;
				// 애니메이션 적용
				InteractionUI->TextOnAnimPlay();
			}
		}
		else if (ActorHasTag("Customizing"))
		{
			InteractionUI->SetActiveWidgetIndex(2);
			if (!bIsInteractionUI)
			{
				bIsInteractionUI = true;
				// 애니메이션 적용
				InteractionUI->TextOnAnimPlay();

			}
		}
		else if (ActorHasTag("PlazaTeleport"))
		{
			InteractionUI->SetActiveWidgetIndex(3);
			if (!bIsInteractionUI)
			{
				bIsInteractionUI = true;
				// 애니메이션 적용
				InteractionUI->TextOnAnimPlay();

			}
		}
	}

	// WidgetComp(삭제)
	// WidgetComp->SetVisibility(true);
	// // GetWidget()을 사용하여 위젯 인스턴스를 가져옴
	// UUserWidget* WidgetCompUI = Cast<UUserWidget>(WidgetComp->GetWidget());
	// if ( WidgetCompUI )
	// {
	// 	// 위젯 인스턴스를 UMH_Interaction으로 캐스팅
	// 	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(WidgetCompUI);
	// 	
	// 	if ( InteractionUI )
	// 	{
	// 		if (ActorHasTag("SelectConcert"))
	// 		{
	// 			InteractionUI->SetActiveWidgetIndex(1);
	// 		}
	// 		else if(ActorHasTag("Customizing"))
	// 		{
	// 			InteractionUI->SetActiveWidgetIndex(2);
	// 		}
	// 		else if(ActorHasTag("PlazaTeleport"))
	// 		{
	// 			InteractionUI->SetActiveWidgetIndex(3);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp , Warning , TEXT("No interaction UI: %s") , *GetName());
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp , Warning , TEXT("No widget found in component: %s") , *GetName());
	// }
}

void AHJ_Actor::HideText()
{
	// 애니메이션 적용 안 함
	//MainUI->SetVisibleInteractionCan(false);

	// 애니메이션 적용
	// // MainUI 체크
	// if (!IsValid(MainUI))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("MainUI is not valid in HideText"));
	// 	return;
	// }
	//
	// // WBP_InteractionUI 체크
	// if (!IsValid(MainUI->WBP_InteractionUI))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("WBP_InteractionUI is not valid in HideText"));
	// 	return;
	// }
	//
	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(MainUI->WBP_InteractionUI);
	// if (!IsValid(InteractionUI))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Failed to cast to UMH_Interaction in HideText"));
	// 	return;
	// }
	//
	if (bIsInteractionUI)
	{
		InteractionUI->TextOffAnimPlay();
		bIsInteractionUI = false;
	}
	// WidgetComp(삭제)
	// WidgetComp->SetVisibility(false);
}

void AHJ_Actor::SetMainUI(UMainWidget* InMainUI)
{
	MainUI = InMainUI;
}
