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
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChattingClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ChatMessageClass;
    
	void AddChatting();
	void AddChatMessage(const FString& Message);
};
