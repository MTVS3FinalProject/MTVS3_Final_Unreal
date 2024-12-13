// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_UserCoinWidget.generated.h"

/**
 * 
 */
//코인충전 버튼
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBuyCoins);
UCLASS()
class MTVS3_FINAL_API UMH_UserCoinWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnClickedBuyCoins OnClickedBuyCoins;
	
	UFUNCTION()
	void OnClickedBuyCoinsBtn()
	{
		// 델리게이트 호출
		OnClickedBuyCoins.Broadcast();
	}
	
	virtual void NativeConstruct() override;

	//내 현재 코인 
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_UserCoin;
	UFUNCTION()
	void SetUserCoin(int32 UserCoin);

	// 델리게이트 콜백 함수
	UFUNCTION()
	void OnCoinValueChanged(int32 NewCoin);
	
	void NativeDestruct();

	UPROPERTY()
	class UTTGameInstance* gi;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyCoins;
};
