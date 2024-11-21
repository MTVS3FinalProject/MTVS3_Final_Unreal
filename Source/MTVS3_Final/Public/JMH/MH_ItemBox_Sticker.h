// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Sticker.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered_Sticker , UMH_ItemBox_Sticker* , HoveredItem_Sticker);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnHovered_Sticker , UMH_ItemBox_Sticker* , UnHoveredItem_Sticker);

UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Sticker : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	// 아이템 박스 버튼에 마우스 호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Sticker OnItemHovered_Sticker;

	UFUNCTION()
	void OnButtonHovered_Sticker()
	{
		OnItemHovered_Sticker.Broadcast(this); // 델리게이트 호출
	}

	// 아이템 박스 버튼에 마우스 언호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemUnHovered_Sticker OnItemUnHovered_Sticker;

	UFUNCTION()
	void OnButtonUnHovered_Sticker()
	{
		OnItemUnHovered_Sticker.Broadcast(this); // 델리게이트 호출
	}

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Sticker;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Sticker;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Sticker;
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Can_StickerInfo;

	UFUNCTION()
	void OnClickedStickerBtn();
	
	UFUNCTION()
	void ChangeColorStickerName(const FString& StickerRarity);
	
	//스티커 설명
	UPROPERTY()
	FString InfoTextString_Sticker;
	
	UFUNCTION()
	void SetInfoString_Sticker(FString infoString_Sticker);

	UFUNCTION()
	FString GetInfoString_Sticker();
	
	UFUNCTION()
	void ShowInfo_Sticker();
	UFUNCTION()
	void HideInfo_Sticker();

	//타이틀 등급
	UFUNCTION()
	void SetStickerRarity(FString StickerRarity);
	
	UFUNCTION()
	FString GetStickerRarity();
	
	UPROPERTY()
	FString RarityString_Sticker;
	
};
