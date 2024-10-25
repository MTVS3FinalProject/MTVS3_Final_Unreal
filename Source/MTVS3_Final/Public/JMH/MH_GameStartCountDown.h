// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_GameStartCountDown.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_GameStartCountDown : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;

	UPROPERTY()
	int32 CountdownValue;
	//카운트다운
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Tex_CountDown;
	UFUNCTION()
	void StartCountdown();
	UFUNCTION()
	void UpdateCountdown();

	UPROPERTY()
	FTimerHandle CountdownTimerHandle;
	
	//애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CountDownAnim;
};
