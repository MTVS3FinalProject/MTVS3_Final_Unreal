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

	void ReqPostVerifySignin(long UserId);

	void OnResPostVerifySignin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostOnVerifySignin(long UserId);

	void OnResPostOnVerifySignin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostSignin(bool bIsHost, FText Email, FText Password, FText Age, FString Gender, FText Nickname);

	void OnResPostSignin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostLogin(FText Email, FText Password);

	void OnResPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostJoinTTSession(long UserId, int64 TTSessionId);

	void OnResPostJoinTTSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostApplyForSeat(long UserId, int64 SeatId);

	void OnResPostApplyForSeat(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostVerifyBooking(long UserId, int64 SeatId);

	void OnResPostVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostOnVerifyBooking(long UserId, int64 SeatId);

	void OnResPostOnVerifyBooking(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReqPostCompleteBooking(long UserId, int64 SeatId);

	void OnResPostCompleteBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);
};
