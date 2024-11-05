// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketCustom.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/Button.h"

class UImage;
class UTextBlock;

void UHM_TicketCustom::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ResetSticker01->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetSticker01Button);
	Btn_ResetSticker02->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetSticker02Button);
	Btn_ResetBackground->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetBackgroundButton);
	Btn_Save->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedSaveButton);
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedExitButton);

	SetupDraggableImage(Img_Sticker01);
	SetupDraggableImage(Img_Sticker02);
	SetupDraggableImage(Img_Sticker03);
	SetupDraggableImage(Img_Sticker04);
	SetupDraggableImage(Img_Sticker05);
	
	bIsDragging = false;
}

void UHM_TicketCustom::SetupDraggableImage(UImage* Image)
{
	if (Image)
	{
		///Image->OnMouseButtonDown.AddDynamic(this, &UHM_TicketCustom::NativeOnMouseButtonDown);
		//Image->OnMouseMove.AddDynamic(this, &UHM_TicketCustom::NativeOnMouseMove);
		//Image->OnMouseButtonUp.AddDynamic(this, &UHM_TicketCustom::NativeOnMouseButtonUp);
	}
}

FReply UHM_TicketCustom::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = true;
		//CurrentImage = Cast<UImage>(MouseEvent.GetEffectingButton());
		DragOffset = MouseEvent.GetScreenSpacePosition() - MyGeometry.GetAbsolutePosition();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UHM_TicketCustom::NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsDragging && CurrentImage)
	{
		FVector2D NewPosition = MouseEvent.GetScreenSpacePosition() - DragOffset;
		CurrentImage->SetRenderTranslation(NewPosition);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


FReply UHM_TicketCustom::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = false;
        CurrentImage = nullptr;  // 드래그 종료
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UHM_TicketCustom::SetRotation(FWidgetTransform NewRotation)
{
	this->SetRenderTransform(NewRotation);
}

void UHM_TicketCustom::SetScale(FVector2D NewScale)
{
	this->SetRenderScale(NewScale);
}

void UHM_TicketCustom::OnClickedResetSticker01Button()
{
	// 스티커 이미지1 리셋 통신 요청
}

void UHM_TicketCustom::OnClickedResetSticker02Button()
{
	// 스티커 이미지2 리셋 통신 요청
}

void UHM_TicketCustom::OnClickedResetBackgroundButton()
{
	// 배경 이미지 리셋 통신 요청
}

void UHM_TicketCustom::OnClickedResetTicketImageButton()
{
	// 티켓 이미지 전체 초기화
}

void UHM_TicketCustom::OnClickedSaveButton()
{
	// 티켓 최종 이미지 서버에 보내기
}

void UHM_TicketCustom::OnClickedExitButton()
{
	// 메인 위젯에서 스위쳐 호출
	//this->SetVisibility(ESlateVisibility::Hidden);
}
