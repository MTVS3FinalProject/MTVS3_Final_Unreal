// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Title.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoubleClicked);

UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Title : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Title;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Title;

	//UFUNCTION()
	//void SetTitleData(const FTitleItemData& InTitleData);

	UFUNCTION()
	void OnClickedTitleBtn();

public:
	// 더블 클릭 델리게이트 선언
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnDoubleClicked OnDoubleClicked;

	// 더블 클릭 이벤트 함수 오버라이드

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);
	
};
