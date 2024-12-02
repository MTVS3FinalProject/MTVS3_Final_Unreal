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

	const FString _url = "https://ticketaka.shop/api";
	
	// QR코드 요청을 서버에 보내는 함수
	void ReqPostGetVerifyIdentityQR(FText Email, FText Password);
	void OnResPostGetVerifyIdentityQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 신원 확인 요청을 서버에 보내는 함수
	void ReqPostVerifyIdentity(FText Email);
	void OnResPostVerifyIdentity(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 회원가입 요청을 서버에 보내는 함수
	void ReqPostSignup(FText Nickname, bool bIsHost , FText Email , FText Password , FString Birth , int32 AvataData);
	void OnResPostSignup(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful);

	// 로그인 요청을 서버에 보내는 함수
	void ReqPostLogin(FText Email, FText Password);
	void OnResPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
