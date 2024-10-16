// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"
#include "JMH/MH_StartWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHM_HttpActor2::AHM_HttpActor2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHM_HttpActor2::BeginPlay()
{
	Super::BeginPlay();
	
	if ( UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("TTLobbyMap") )
	{
		// 특정 레벨일 때 실행할 코드
		UE_LOG(LogTemp , Warning , TEXT("현재 레벨은 TTLobbyMap입니다."));

		StartUI = CastChecked<UMH_StartWidget>(CreateWidget(GetWorld() , StartUIFactory));
		if ( StartUI )
		{
			StartUI->AddToViewport();
		}

		auto* pc = UGameplayStatics::GetPlayerController(this , 0);
		if ( !pc ) return;
		pc->SetShowMouseCursor(true);
		pc->SetInputMode(FInputModeGameAndUI());
	}
}

// Called every frame
void AHM_HttpActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

