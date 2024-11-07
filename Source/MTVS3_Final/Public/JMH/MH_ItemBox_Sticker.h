// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Sticker.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Sticker : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Sticker;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Sticker;

	UFUNCTION()
	void SetStickerData(int32 StickerID, FString StickerScript, UTexture2D* StickerImg);

	
	UFUNCTION()
	void OnClickedStickerBtn();
	
};
