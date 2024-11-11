// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_EmojiWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_EmojiWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emotion1;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emotion2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emotion3;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emotion4;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emotion5;

	UFUNCTION()
	void OnClickedEmotion1Bun();
	
	UFUNCTION()
	void OnClickedEmotion2Bun();
	
	UFUNCTION()
	void OnClickedEmotion3Bun();
	
	UFUNCTION()
	void OnClickedEmotion4Bun();
	
	UFUNCTION()
	void OnClickedEmotion5Bun();

	UFUNCTION()
	void HideEmojiWidget();

	UPROPERTY()
	bool bIsEmojiAnimating = false;
};
