// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_BuyTicketWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_BuyTicketWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;

		
	//Start
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_BuyTicketSwitcher;
	
	//위젯스위처설정
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm1;
	UFUNCTION()
	void OnClickedConfirm1Button();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back01;
	UFUNCTION()
	void OnClickedBack01Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_MySeatMap;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* ETex_PhoneNum;
	
};
