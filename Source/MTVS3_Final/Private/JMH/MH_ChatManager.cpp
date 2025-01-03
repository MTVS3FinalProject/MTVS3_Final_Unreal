// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ChatManager.h"

// Sets default values
AMH_ChatManager::AMH_ChatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 서버와 클라이언트 모두에서 존재하게 설정
}

// Called when the game starts or when spawned
void AMH_ChatManager::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		SetOwner(PlayerController);
	}
}

// Called every frame
void AMH_ChatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMH_ChatManager::SendMessage(const FString& chatMessage)
{
	if(HasAuthority())
	{
		//서버에서 발생한 메세지: 모든 클라에 전달
		MultiReceiveChatMessage(chatMessage);
	}
	else
	{
		//클라에서 발생한 메세지: 서버로 전송
		ServerSendChatMessage(chatMessage);
	}
}

bool AMH_ChatManager::ServerSendChatMessage_Validate(const FString& ServerMessage)
{
	// 유효성 검사 로직 예시: 메시지가 비어 있지 않아야 함
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("444"));
	return !ServerMessage.IsEmpty();
}

void AMH_ChatManager::ServerSendChatMessage_Implementation(const FString& ServerMessage)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("555"));
	// 서버에서 받은 메시지를 모든 클라이언트에게
	MultiReceiveChatMessage(ServerMessage);
}
void AMH_ChatManager::MultiReceiveChatMessage_Implementation(const FString& ChatMessage)
{
	OnMessageReceived.Broadcast(ChatMessage);
}
