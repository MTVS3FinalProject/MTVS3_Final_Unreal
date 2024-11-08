// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_ChatManager.generated.h"

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Message);

UCLASS()
class MTVS3_FINAL_API AMH_ChatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMH_ChatManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FOnMessageReceived OnMessageReceived;
	
	//메시지 클라이언트들에게 전송하는 서버 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& ServerMessage);

	//메시지를 각 클라와 서버에
	UFUNCTION(NetMulticast,Reliable)
	void MultiReceiveChatMessage(const FString& ChatMessage);

	//로컬에서 메세지 전송	
	UFUNCTION()
	void SendMessage(const FString& chatMessage);
	
};
