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

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CountValue")
	int32 CountdownValue;
	
	//카운트다운
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_CountDown;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Can_StartCountDown;
	UFUNCTION(BlueprintCallable)
	void StartCountdown();
	UFUNCTION()
	void UpdateCountdown();
	//UFUNCTION()
	//void StartCountDownVisible(bool visible);

	UPROPERTY()
	FTimerHandle CountdownTimerHandle;
	
	//애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CountDownAnim;

	// 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundWave* CountdownSound;
};
