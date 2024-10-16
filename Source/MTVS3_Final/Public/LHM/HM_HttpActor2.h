// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpFwd.h"
#include "HM_HttpActor2.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_HttpActor2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_HttpActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
#pragma region UI
	UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	TSubclassOf<class UMH_TicketingWidget> TicketingUIFactory;
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;
#pragma endregion

	// 백엔드에 요청 보낼 때만 api 포함, 프론트는 api X
	const FString _url = "https://ticketaka.shop/api";

//===========================================================================================================

	// 좌석 접수 조회 요청
	void ReqGetSeatRegistrationCount(const FString& SeatId , const FString& AccessToken);

	// 좌석 접수 조회 요청에 대한 응답
	void OnResGetSeatRegistrationCount(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 접수 요청
	void ReqPostRegisterSeat(const FString& SeatId , const FString& AccessToken);

	// 좌석 접수 요청에 대한 응답
	void OnResPostRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청
	void ReqDeleteCancelSeat(const FString& SeatId , const FString& AccessToken);

	// 좌석 취소 요청에 대한 응답
	void OnResDeleteCancelSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
};
