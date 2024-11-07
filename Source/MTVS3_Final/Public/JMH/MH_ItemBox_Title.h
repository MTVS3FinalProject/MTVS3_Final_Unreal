// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Title.generated.h"

/**
 * 
 */

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

	UFUNCTION()
	void SetTitleData(int32 TitleID, FString TitleName, UTexture2D* TitleImg);

	UFUNCTION()
	void OnClickedTitleBtn();
	
};
