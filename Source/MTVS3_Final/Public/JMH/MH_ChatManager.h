// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_ChatManager.generated.h"

USTRUCT()
struct FChatMessege
{
	GENERATED_BODY()

	UPROPERTY()
	FString SenderName;

	UPROPERTY()
	FString Message;

	FChatMessege() {}

	FChatMessege(const FString& InSenderName, const FString& InMessage): SenderName(InSenderName),Message(InMessage){}
	
	
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
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& SenderName, const FString& Message);

	UFUNCTION(Client,Reliable)
	void ClientReceiveChatMessage(const FChatMessege ChatMessege);

	
};
