// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketBG.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

void UHM_TicketBG::NativeConstruct()
{
	Super::NativeConstruct();

	//RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if( Img_TicketBackground && Img_TicketInfo && Overlay_TicketBG )
	{
		// 에디터에서 설정된 위치와 크기 사용
		UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(Img_TicketBackground->Slot);
		UCanvasPanelSlot* InfoSlot = Cast<UCanvasPanelSlot>(Img_TicketInfo->Slot);
		UCanvasPanelSlot* OverlaySlot = Cast<UCanvasPanelSlot>(Overlay_TicketBG->Slot);
		
		if (BackgroundSlot)
		{
			BackgroundSlot->SetSize(FVector2D(844, 500));
			BackgroundSlot->SetPosition(FVector2D(0));
			BackgroundSlot->SetAlignment(FVector2d(0.5));
		}

		if (InfoSlot)
		{
			InfoSlot->SetSize(FVector2D(436, 500));
			InfoSlot->SetPosition(FVector2D(640, 0));
			InfoSlot->SetAlignment(FVector2d(0.5));
		}
		
		if (OverlaySlot)
		{
			OverlaySlot->SetSize(FVector2D(844, 500));
			OverlaySlot->SetPosition(FVector2D(0));
			OverlaySlot->SetAlignment(FVector2d(0.5));
		}
	}
}
