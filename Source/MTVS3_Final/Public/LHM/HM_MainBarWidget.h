// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_MainBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_MainBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_Bar;

	UFUNCTION()
	void SetVisibleSwitcher(bool bVisible);

	UFUNCTION()
	void SetWidgetSwitcher(int32 num);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Emoji;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_CollectionBook;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Notice;
	
	bool bIsEmojiiVisible = false;
	bool bIsCollectionBookVisible = false;
	bool bIsNoticeVisible = false;

	UFUNCTION()
	void OnClickedEmojiBtn();
	
	UFUNCTION()
	void OnClickedCollectionBookBtn();
	
	UFUNCTION()
	void OnClickedNoticeBtn();
};
