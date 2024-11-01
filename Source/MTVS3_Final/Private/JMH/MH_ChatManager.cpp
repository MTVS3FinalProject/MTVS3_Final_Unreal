// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ChatManager.h"

// Sets default values
AMH_ChatManager::AMH_ChatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMH_ChatManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMH_ChatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMH_ChatManager::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	// 서버에서 받은 메시지를 모든 클라이언트에게 브로드캐스트
	BroadcastChatMessage(SenderName, Message);
}

bool AMH_ChatManager::ServerSendChatMessage_Validate(const FString& SenderName, const FString& Message)
{
	// 유효성 검사 로직 예시: 메시지가 비어 있지 않아야 함
	return !Message.IsEmpty();
}

void AMH_ChatManager::ClientReceiveChatMessage_Implementation(const FChatMessege ChatMessege)
{
	
}
