// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_FinalTicket.h"

#include "Components/Image.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LHM/HM_TicketBG.h"
#include "Slate/WidgetRenderer.h"

class FWidgetRenderer;

void UHM_FinalTicket::NativeConstruct()
{
	Super::NativeConstruct();

	// ImgTicketBackgroundWidget의 인스턴스 생성
	if (TicketBGWidget)
	{
		TicketBGUI = CreateWidget<UHM_TicketBG>(GetWorld(), TicketBGWidget);
	}
}

void UHM_FinalTicket::CaptureAndDisplayTicketBackground()
{
	if (!TicketBGUI) return;

	// FWidgetRenderer 생성
	TSharedPtr<FWidgetRenderer> WidgetRenderer = MakeShareable(new FWidgetRenderer(true));

	// Render Target 생성 및 크기 설정
	//FVector2D CaptureSize = TicketBGUI->Img_TicketBackground->GetDesiredSize();
	CaptureSize = FVector2D(844.0f, 500.0f); // 기본 크기 설정
	TicketBGUI->Img_TicketBackground->SetDesiredSizeOverride(CaptureSize);
	
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitCustomFormat(CaptureSize.X, CaptureSize.Y, PF_B8G8R8A8, true);

	// ImgTicketBackgroundWidgetInstance 캡처
	WidgetRenderer->DrawWidget(RenderTarget, TicketBGUI->TakeWidget(), CaptureSize, 1.0f);

	// Render Target을 UTexture2D로 변환
	UTexture2D* CapturedTexture = ConvertRenderTargetToTexture(this, RenderTarget);
	if (CapturedTexture)
	{
		CapturedTexture->UpdateResource(); // 리소스 업데이트

		// Img_FinalTicket에 캡처된 텍스처 표시
		if (Img_FinalTicket)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(CapturedTexture);
			Img_FinalTicket->SetBrush(Brush);
		}
	}
}

class UTexture2D* UHM_FinalTicket::ConvertRenderTargetToTexture(UObject* WorldContextObject,
	UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget) return nullptr;

	UTexture2D* NewTexture = NewObject<UTexture2D>(WorldContextObject);
	UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(WorldContextObject, RenderTarget, NewTexture);

	return NewTexture;
}
