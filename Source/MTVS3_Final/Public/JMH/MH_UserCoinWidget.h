// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_UserCoinWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_UserCoinWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	//코인 충전버튼
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyCoins;
	UFUNCTION()
	void OnClickedBuyCoinsButton();

	
	
};
