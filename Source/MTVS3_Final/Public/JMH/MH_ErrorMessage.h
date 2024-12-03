// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ErrorMessage.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedErrorExit);

UCLASS()
class MTVS3_FINAL_API UMH_ErrorMessage : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
public:
	FOnClickedErrorExit OnClickedErrorExit;

	UFUNCTION()
	void CloseBtnPressedErrorExit()
	{
		// 델리게이트 호출
		OnClickedErrorExit.Broadcast();
	}
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ErrorMessage;

	UFUNCTION()
	void SetTextMessage(FString ErrorMS);
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_ErrorExit;
	
	UFUNCTION()
	void ShowErrorMessage(FString ErrorMS);
	
};
