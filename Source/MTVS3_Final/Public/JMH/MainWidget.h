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
	
	//티켓예매 알람
	//티켓팅 시간대
	//상호작용 버튼
	
};
