// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Message.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_Message : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Message;

	void SetChatMessage(const FString& Message);
};
