// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_Chatting.h"
#include "GameFramework/Actor.h"
#include "MH_ChatManager.generated.h"

USTRUCT()
struct FChatMessage
{
	GENERATED_BODY()
	UPROPERTY()
	FString Message;

	FChatMessage() {}

	FChatMessage(const FString& InMessage): Message(InMessage){}
	
	
};
UCLASS()
class MTVS3_FINAL_API AMH_ChatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMH_ChatManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//메시지 클라이언트들에게 전송하는 서버 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& Message);

	//메시지를 각 클라
	UFUNCTION(NetMulticast,Reliable)
	void MultiReceiveChatMessage(const FChatMessage ChatMessage);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMH_Chatting> ChatWidget;

	UPROPERTY()
	class UMH_Chatting* ChatUI;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMH_Message> Message;

	UPROPERTY()
	class UMH_Message* MessageUI;
	
	UFUNCTION(Blueprintable)
	void AddChatting();
	UFUNCTION(Blueprintable)
	void AddChatMessage(const FString& Message);

	
	
};
