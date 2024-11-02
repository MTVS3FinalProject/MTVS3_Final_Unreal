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
	virtual void BeginPlay() override;
	

public:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMH_Message> MessageWidget;

	UFUNCTION()
	void AddChatting();
UFUNCTION()
	void AddChatMessage(const FString& Message);

	UPROPERTY()
	APlayerController* OwningPlayer = nullptr;
	
};
