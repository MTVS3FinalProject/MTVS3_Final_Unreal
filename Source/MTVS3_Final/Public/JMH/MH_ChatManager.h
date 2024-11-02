// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_Chatting.h"
#include "GameFramework/Actor.h"
#include "MH_ChatManager.generated.h"

UCLASS()
class MTVS3_FINAL_API AMH_ChatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY()
	FString Message;
	
	AMH_ChatManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	//메시지 클라이언트들에게 전송하는 서버 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& ServerMessage);

	//메시지를 각 클라
	UFUNCTION(NetMulticast,Reliable)
	void MultiReceiveChatMessage(const FString& ChatMessage);
	
};
