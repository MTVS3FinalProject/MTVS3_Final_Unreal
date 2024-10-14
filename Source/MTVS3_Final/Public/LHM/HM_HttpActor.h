// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpFwd.h"
#include "HM_HttpActor.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_HttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_HttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	#pragma region UI
	UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	TSubclassOf<class UMH_StartWidget> StartUIFactory;
	UPROPERTY()
	class UMH_StartWidget* StartUI;
#pragma endregion

	const FString _url = "http://121.165.108.133:7878/api";

//===========================================================================================================

	// QR코드 요청을 서버에 보내는 함수
	void ReqPostGetVerifyIdentityQR(FText Email);

	// QR코드 요청에 대한 응답을 처리하는 함수
	void OnResPostGetVerifyIdentityQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 신원 확인 요청을 서버에 보내는 함수
	void ReqPostVerifyIdentity(FText Email);

	// 신원 확인 요청에 대한 응답 처리하는 함수
	void OnResPostVerifyIdentity(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 회원가입 요청을 서버에 보내는 함수
	void ReqPostSignup(bool bIsHost , FText Email , FText Password , FString Birth , FText Nickname, int32 AvataData);

	// 회원가입 요청에 대한 응답을 처리하는 함수
	void OnResPostSignup(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

//===========================================================================================================

	// 로그인 요청을 서버에 보내는 함수
	void ReqPostLogin(FText Email, FText Password);

	// 로그인 요청에 대한 응답을 처리하는 함수
	void OnResPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

//===========================================================================================================

	// TT세션 입장 요청을 서버에 보내는 함수
	void ReqPostJoinTTSession(int32 UserId, int64 TTSessionId);

	// TT세션 입장 요청에 대한 응답을 처리하는 함수
	void OnResPostJoinTTSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

//===========================================================================================================

	// 좌석에 접수 신청하는 요청을 서버에 보내는 함수
	void ReqPostApplyForSeat(int32 UserId, int64 SeatId);

	// 좌석 접수 신청하는 요청에 대한 응답을 처리하는 함수
	void OnResPostApplyForSeat(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 좌석 예약을 검증하는 요청을 서버에 보내는 함수
	//void ReqPostVerifyBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 검증 요청에 대한 응답을 처리하는 함수
	//void OnResPostVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 좌석 예약 검증 결과를 확인하는 요청을 서버에 보내는 함수
	//void ReqPostOnVerifyBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 검증 결과를 확인하는 요청에 대한 응답을 처리하는 함수
	//void OnResPostOnVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 좌석 예약 완료 요청을 서버에 보내는 함수
	//void ReqPostCompleteBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 완료 요청에 대한 응답을 처리하는 함수
	//void OnResPostCompleteBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 예약 취소 요청을 서버에 보내는 함수
	void ReqPostCancleBooking(int32 UserId , int64 SeatId);

	// 좌석 예약 취소 요청에 대한 응답을 처리하는 함수
	void OnResPostCancleBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	//===========================================================================================================

	// 티켓 예매 시 QR코드 요청을 서버에 보내는 함수
	void ReqPostGetTicketQR(int32 UserId , int64 SeatId , FText Name , FText PhoneNum , int32 BirthDate , FText Address);

	// 티켓 예매 시 QR코드 요청에 대한 응답을 처리하는 함수
	void OnResPostGetTicketQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 티켓 예매 시 신원 확인 요청을 서버에 보내는 함수
	void ReqPostVerifyIdentityTicketQR(int32 UserId);

	// 티켓 예매 시 신원 확인 요청에 대한 응답을 처리하는 함수
	void OnResPostVerifyIdentityTicketQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 코인 충전 요청을 서버에 보내는 함수
	void ReqPostCoinCharge(int32 UserId , int32 Coin);

	// 코인 충전 요청에 대한 응답을 처리하는 함수
	void OnResPostCoinCharge(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 티켓 예매 시 코인 결제 요청을 서버에 보내는 함수
	void ReqTicketCoinPayment(int32 UserId , int32 Coin);

	// 티켓 예매 시 코인 결제 요청에 대한 응답을 처리하는 함수
	void OnResTicketCoinPayment(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
};
