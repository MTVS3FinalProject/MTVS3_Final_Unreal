// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Sticker.generated.h"

/**
 * 
 */

//struct FTitleItemData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered_Sticker, bool, bHovered);

UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Sticker : public UUserWidget
{
	GENERATED_BODY()

public:
	// 아이템 박스 버튼에 마우스 호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Sticker OnItemHovered_Sticker;

	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonHovered_Sticker();
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Sticker;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Sticker;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Sticker;

	//UFUNCTION()
	//void SetStickerData(const FStickerItemData& InStickerData);

	
	UFUNCTION()
	void OnClickedStickerBtn();
	
};
