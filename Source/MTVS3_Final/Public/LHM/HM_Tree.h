// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "HM_Tree.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_Tree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_Tree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Tree;

	UPROPERTY(EditDefaultsOnly, Replicated)
	TArray<UStaticMeshComponent*> Ticats;
	
	UPROPERTY(ReplicatedUsing=OnRep_TicatVisibility)
	TArray<bool> TicatVisibilities; // Ticat의 가시성을 관리하는 배열

	UFUNCTION()
	void OnRep_TicatVisibility();

	// 트리 조회 : 로그인할 때 각 클라이언트에서 실행 동기화X
	UFUNCTION()
	void InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg);

	// 트리에 티켓 달기
	UFUNCTION(Client, Reliable)
	void Client_ApplyTicketImage(int32 TicketTreeId, const FString& TicketImgUrl);
	UFUNCTION(Server, Reliable)
	void Server_ApplyTicketImage(int32 TicketTreeId, const FString& TicketImgUrl);
	
};
