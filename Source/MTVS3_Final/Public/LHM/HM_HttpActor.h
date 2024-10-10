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

	/*#pragma region UI
	UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	TSubclassOf<class UStartWidget> StartUIFactory;
	UPROPERTY()
	class UStartWidget* StartUI;
#pragma endregion*/


//===========================================================================================================

	// 회원가입 요청을 서버에 보내는 함수1
	void ReqPostSignup1(bool bIsHost, FText Email, FText Password, int32 Age/*, FString UserId*/);

	// 회원가입 요청에 대한 응답을 처리하는 함수1
	void OnResPostSignup1(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 신원 확인을 요청하는 함수
	void ReqPostVerifyIdentity(int32 UserId);

	// 신원 확인 요청에 대한 응답 처리하는 함수
	void OnResPostVerifyIdentity(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 신원 확인 결과 확인 요청을 서버에 보내는 함수
	void ReqPostOnVerifyIdentity(int32 UserId);

	// 신원 확인 결과 확인 요청에 대한 응답을 처리하는 함수
	void OnResPostOnVerifyIdentity(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 회원가입 요청을 서버에 보내는 함수2
	void ReqPostSignup2(FText Nickname, int32 CharacterModel);

	// 회원가입 요청에 대한 응답을 처리하는 함수2
	void OnResPostSignup2(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 로그인 요청을 서버에 보내는 함수
	void ReqPostLogin(FText Email, FText Password);

	// 로그인 요청에 대한 응답을 처리하는 함수
	void OnResPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

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
	void ReqPostVerifyBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 검증 요청에 대한 응답을 처리하는 함수
	void OnResPostVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 좌석 예약 검증 결과를 확인하는 요청을 서버에 보내는 함수
	void ReqPostOnVerifyBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 검증 결과를 확인하는 요청에 대한 응답을 처리하는 함수
	void OnResPostOnVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 좌석 예약 완료 요청을 서버에 보내는 함수
	void ReqPostCompleteBooking(int32 UserId, int64 SeatId);

	// 좌석 예약 완료 요청에 대한 응답을 처리하는 함수
	void OnResPostCompleteBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 좌석 예약 취소 요청을 서버에 보내는 함수
	void ReqPostCancleBooking(int32 UserId , int64 SeatId);

	// 좌석 예약 취소 요청에 대한 응답을 처리하는 함수
	void OnResPostCancleBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
};
