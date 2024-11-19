// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Interaction.generated.h"

/**
 * 
 */

UCLASS()
class MTVS3_FINAL_API UMH_Interaction : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_InteractionWidgetSwitcher;

	UFUNCTION()
	void SetActiveWidgetIndex(int index);//0:Chair, 1:콘서트 입장, 2:스타일 라운지
	
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* TextOffAnim;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* TextOnAnim;

	UPROPERTY()
	bool bAnimationFinished = false;

	UFUNCTION()
	void TextOnAnimPlay();

	UFUNCTION()
	void TextOffAnimPlay();
	
	virtual void OnAnimationFinished();
	
};
