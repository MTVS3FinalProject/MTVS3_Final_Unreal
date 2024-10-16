// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_GameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_GameWidgetSwitcher;

	//위젯스위처설정
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);

	//룰렛1
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_roulette1;
	
	UFUNCTION()
	void SetTextroulette1(FString roulette1);
	
};
