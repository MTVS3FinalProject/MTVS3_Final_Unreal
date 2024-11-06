// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemInfoBox.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_ItemInfoBox : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ItemInfo;

	UFUNCTION()
	void SetTextItemInfo(FString Info);
	
};
