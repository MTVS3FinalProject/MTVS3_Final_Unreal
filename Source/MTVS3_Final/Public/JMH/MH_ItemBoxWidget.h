// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_ItemBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item;

	UFUNCTION()
	void SetItemImg(UTexture2D* ItemImg);

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item;
	
	
};
