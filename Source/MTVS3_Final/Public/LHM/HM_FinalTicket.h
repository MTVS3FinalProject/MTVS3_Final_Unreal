// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_FinalTicket.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_FinalTicket : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UCanvasPanel* RootCanvas;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_FinalTicket; // 캡처 결과를 표시할 이미지
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_FinalTicketInfo; // 캡처 결과를 표시할 이미지
	
	FVector2d CaptureSize;
	
	// 배경 티켓 이미지를 캡처하고 DisplayImage에 표시하는 함수
	UFUNCTION()
	void CaptureAndDisplayTicketBackground(class UHM_TicketCustom* _TicketCutomUI);

	// 텍스쳐를 PNG데이터로 변환하는 함수
	UFUNCTION()
	TArray<uint8> ConvertTextureToPNG(UTextureRenderTarget2D* RenderTarget);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UHM_TicketCustom> TicketCutomWidget; // 배경 티켓 위젯 클래스 참조
	class UHM_TicketCustom* TicketCutomUI; // 배경 티켓 위젯 인스턴스
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UMainWidget* MainUI;
	void SetMainUI(UMainWidget* InMainUI);
	
private:
	class UTexture2D* ConvertRenderTargetToTexture(UObject* WorldContextObject, UTextureRenderTarget2D* RenderTarget);
};
