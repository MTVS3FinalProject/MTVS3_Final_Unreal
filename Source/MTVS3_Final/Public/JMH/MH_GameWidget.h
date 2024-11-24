// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_GameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))//0:현재인원,타이머/1: 룰렛 / 2: 축하~
	class UWidgetSwitcher* WS_GameWidgetSwitcher;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_01;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_02;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_03;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_04;
	
	//위젯스위처설정/ 0:게임시작,1: 룰렛결과,
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);
	
	//Hide
	UFUNCTION()
	void HideWidget();
	
	//Show
	UFUNCTION()
	void ShowWidget();

	
//InGame
	//현재 인원
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_CurrentPlayer;
	UFUNCTION()
	void SetTextCurrentPlayer(int32 CurrentPlayer);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_CountDown;
	UFUNCTION()
	void SetTextCountDown(FString CountDownTime);
	
	//룰렛1
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_roulette1;
	UFUNCTION()
	void SetTextroulette1(FString roulette1);

	//룰렛2
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_roulette2;
	UFUNCTION()
	void SetTextroulette2(int32 roulette2);

	//룰렛3
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_roulette3;
	UFUNCTION()
	void SetTextroulette3(int32 roulette3);

	//애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Roulette01Anim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Roulette02Anim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Roulette03Anim;
	//룰렛 내려오는 애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Roulette04Anim;
	UFUNCTION()
	void PlayRouletteAnim04();
	//룰렛 시작 애니메이션 함수
	UFUNCTION()
	void PlayRouletteAnim();
	
	//내 좌석 번호
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_MyNum;
	UFUNCTION()
	void SetTextMyNum(int32 MyNum);
	UFUNCTION()
	void SetOnlyVisibleMyNum(bool bIsVisible);
	
	UFUNCTION()
	void SetCurrentPlayerVisible();
	//처음 어떻게 있을지 정해야함.
	//결과 바뀔때 애니메이션 해야함.
	
	// KHJ
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInAnim;
};
