// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Main;

	UFUNCTION()
	void SetVisibleCanvas(bool bVisible);
	
	//미니맵
	//현재시간 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_CurrentTime;
	UFUNCTION()
	void SetTextCurrentTime(FString CurrentTime);

	//나가기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BackMain;
	UFUNCTION()
	void OnClickedBackMain();
	//티켓예매 알람
	//티켓팅 시간대
	//상호작용 버튼
	
	// ==================================
	// HttpActor2 공연장 입장 통신 테스트용
	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UButton* Btn_GetConcertInfo;
	//UFUNCTION()
	//void OnClickedGetConcertInfo();

	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UButton* Btn_ConcertEntry;
	//UFUNCTION()
	//void OnClickedConcertEntry();

	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UButton* Btn_SeatRegistrationInquiry;
	//UFUNCTION()
	//void OnClickedSeatRegistrationInquiry();

	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UButton* Btn_GameResult;
	//UFUNCTION()
	//void OnClickedGameResult();
};
