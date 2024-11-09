// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HTTPActor_Inven.h"

// Sets default values
AHM_HTTPActor_Inven::AHM_HTTPActor_Inven()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void AHM_HTTPActor_Inven::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHM_HTTPActor_Inven::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 서버로 인벤토리 데이터 요청
void AHM_HTTPActor_Inven::RequestInventoryData()
{
	
}

// 서버 응답 처리 함수
void AHM_HTTPActor_Inven::OnInventoryDataReceived(const FString& JsonResponse)
{
	
}
