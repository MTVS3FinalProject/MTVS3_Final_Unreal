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
	//UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	//TSubclassOf<class UMH_TicketingWidget> TicketingUIFactory;
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;

    void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

#pragma endregion

	// 백엔드에 요청 보낼 때만 api 포함, 프론트는 api X
	const FString _url = "https://ticketaka.shop/api";

//===========================================================================================================

	// 공연장 입장 요청
	void ReqPostConcertEntry(FString ConcertName , FString AccessToken);

	// 공연장 입장 요청에 대한 응답
	void OnResPostConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

//===========================================================================================================

	// 좌석 조회 요청
	void ReqPostSeatRegistrationInquiry(FString SeatId , FString AccessToken);

	// 좌석 조회 요청에 대한 응답
	void OnResPostSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 접수 요청
	void ReqPostRegisterSeat(FString SeatId , FString AccessToken);

	// 좌석 접수 요청에 대한 응답
	void OnResPostRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 취소 요청
	void ReqDeleteCancelRegisteredSeat(FString SeatId , FString AccessToken);

	// 좌석 취소 요청에 대한 응답
	void OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

//===========================================================================================================

	// 좌석 게임 결과 , 응답 필요없음
	void ReqPostGameResult(FString SeatId , FString AccessToken);

	// 결제시 회원 인증용 QR 요청
	void ReqGetMemberAuthQR(FString AccessToken);

	// 결제시 회원 인증용 QR 요청에 대한 응답
	void OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 결제시 회원 인증 사진 업로드 확인
	void ReqGetPostConfirmMemberPhoto(FString AccessToken);

	// 결제시 회원 인증 사진 업로드 확인에 대한 응답
	void OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 예매자 정보 입력 요청
	void ReqPostReservationinfo(FString UserName, int32 UserPhoneNum , FString UserAddress , FString AccessToken);

	// 예매자 정보 입력 요청에 대한 응답
	void OnResPostReservationinfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 결제 요청
	void ReqPostPaymentSeat(FString ConcertName, FString SeatId, FString AccessToken);

	// 좌석 결제 요청에 대한 응답
	void OnResPostPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

};
