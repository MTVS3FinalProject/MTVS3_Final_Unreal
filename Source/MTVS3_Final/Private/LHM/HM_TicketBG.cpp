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

	RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	if( RootCanvas && Img_TicketBackground && Img_TicketInfo && Overlay_TicketBG )
	{
		// 에디터에서 설정된 위치와 크기 사용
		UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(Img_TicketBackground->Slot);
		UCanvasPanelSlot* InfoSlot = Cast<UCanvasPanelSlot>(Img_TicketInfo->Slot);
		UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Overlay_TicketBG->Slot);
		
		if (BackgroundSlot)
		{
			BackgroundSlot->SetSize(FVector2D(844, 500));
			BackgroundSlot->SetPosition(FVector2D(-770, -285));
		}

		if (InfoSlot)
		{
			InfoSlot->SetSize(FVector2D(436, 500));
			InfoSlot->SetPosition(FVector2D(74, -285));
		}
		
		if (OverlaySlot)
		{
			OverlaySlot->SetHorizontalAlignment(HAlign_Center);
			OverlaySlot->SetVerticalAlignment(VAlign_Center);
			
		}
	}
}
