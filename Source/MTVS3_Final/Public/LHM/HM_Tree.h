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

	UPROPERTY(Replicated)
	TArray<UStaticMeshComponent*> Ticats;
	
	UPROPERTY(ReplicatedUsing=OnRep_TicatVisibility)
	TArray<bool> TicatVisibilities; // Ticat의 가시성을 관리하는 배열

	UFUNCTION()
	void OnRep_TicatVisibility();

	UFUNCTION()
	void InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg);

	UFUNCTION(Server, Reliable)
	void Server_RequestInitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg);

	UFUNCTION(Server, Reliable)
	void Server_ApplyTicketImage(const FString& TicketImgUrl);
	
	//UFUNCTION()
	//void ApplyTicketImageFromUrl(const FString& TicketImgUrl);

//private:
	//void OnTicketImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
