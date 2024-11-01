// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MH_TTHUD.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API AMH_TTHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChatWidget;

	UPROPERTY()
	class UMH_Chatting* ChatUI;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MessageWidget;
	
	void AddChatting();
	void AddChatMessage(const FString& Message);

	UPROPERTY()
	APlayerController* OwningPlayer = nullptr;
};
