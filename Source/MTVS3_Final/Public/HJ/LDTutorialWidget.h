// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LDTutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API ULDTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_LDTutorial;
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_Tutorial;

	//위젯스위처설정 
	UFUNCTION()
	void SetWidgetSwitcher(int32 num); //0:추첨 안내, 1:랜덤 번호 배정, 2:룰렛 설명
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_TutorialStart;
	UFUNCTION()
	void OnClickedTutorialStart();
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* Btn_TutorialSkip;
	UFUNCTION()
	void OnClickedTutorialSkip();

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* Btn_Right1;
	UFUNCTION()
	void OnClickedRight1();

	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* Btn_TutorialEnd;
	UFUNCTION()
	void OnClickedTutorialEnd();
};
