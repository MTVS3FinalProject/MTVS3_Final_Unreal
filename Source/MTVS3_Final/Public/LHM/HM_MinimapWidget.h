// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_MinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_MinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_MinimapImg;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_StyleLounge;

	UFUNCTION()
	void UpdateMinimapWidget();
};
