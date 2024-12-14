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
	FVector StyleLounge = FVector(18000.0f, 4900.0f, 3300.0f);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_Community;
	FVector Community = FVector(23560.0f, 2029.0f, 3300.0f);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_Kiosk_1;
	FVector Kiosk_1 = FVector(17318.0f, 1342.0f, 3300.0f);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_Kiosk_2;
	FVector Kiosk_2 = FVector(18652.0f, 1341.0f, 3300.0f);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_Kiosk_3;
	FVector Kiosk_3 = FVector(18657.0f, 2661.0f, 3300.0f);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_Kiosk_4;
	FVector Kiosk_4 = FVector(17358.0f, 2666.0f, 3300.0f);

	UFUNCTION()
	void SetImgsVisible(bool visible);
	
	UFUNCTION()
	void UpdateTargetPosition(FVector TargetLocation, UImage* Img_Target);
};
