// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketCustom.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "UObject/FastReferenceCollector.h"

void UHM_TicketCustom::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ResetBackground->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetBackgroundButton);
	Btn_Save->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedSaveButton);
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedExitButton);

	RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	
	SetupDraggableImage(Img_Sticker01);
	SetupDraggableImage(Img_Sticker02);
	SetupDraggableImage(Img_Sticker03);
	SetupDraggableImage(Img_Sticker04);
	SetupDraggableImage(Img_Sticker05);
	
	bIsDragging = false;
	CurrentImage = nullptr;
    DraggedImageCopy = nullptr;

}

void UHM_TicketCustom::SetupDraggableImage(UImage* Image)
{
	if (Image)
	{
		Image->SetVisibility(ESlateVisibility::Visible);
		Image->bIsVariable = true;
		Image->SetIsEnabled(true);
	}
}

UImage* UHM_TicketCustom::CreateDraggableImageCopy(UImage* SourceImage)
{
	if(!RootCanvas || !SourceImage) return nullptr;

	// 새 이미지 생성
	UImage* NewImage = NewObject<UImage>(this, UImage::StaticClass());
	if (!NewImage) return nullptr;

	// 원본 이미지의 속성 복사
	UTexture2D* TextureResource = Cast<UTexture2D>(SourceImage->GetBrush().GetResourceObject());
	if (!TextureResource) return nullptr;
	
	NewImage->SetBrushFromTexture(TextureResource);
	NewImage->SetDesiredSizeOverride(SourceImage->GetBrush().GetImageSize());
	NewImage->SetColorAndOpacity(SourceImage->GetColorAndOpacity());
	NewImage->SetVisibility(ESlateVisibility::Visible);
	NewImage->bIsVariable = true;
	NewImage->SetIsEnabled(true);

	// 캔버스 패널 슬롯 설정
	UCanvasPanelSlot* CanvasSlot = RootCanvas->AddChildToCanvas(NewImage);
	if (!CanvasSlot) return nullptr;
	
	// SourceImage의 위치와 크기 복사
	UCanvasPanelSlot* SourceSlot = Cast<UCanvasPanelSlot>(SourceImage->Slot);
	if (SourceSlot)
	{
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetPosition(SourceSlot->GetPosition());
		CanvasSlot->SetZOrder(100);
	}
	return NewImage;
}

void UHM_TicketCustom::SetRenderScale(UImage* Image, const FVector2D& MouseDelta)
{
	// 마우스 이동 거리의 크기에 따라 Sclae 조정
	// 임의 조절 비율 0.01
	float ScaleFactor = 1 + MouseDelta.Y * 0.01f;
	Image->SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));
}

void UHM_TicketCustom::SetRenderAngle(UImage* Image, const FVector2D& MouseDelta)
{
	// 마우스 이동에 따라 회전 각도 조정
	// 임의 조절 비율 0.5
	float AngleDelta = MouseDelta.X * 0.5f;
	float CurrentAngle = Image->RenderTransform.Angle;
	Image->SetRenderTransformAngle(CurrentAngle + AngleDelta);
}

FReply UHM_TicketCustom::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		TArray<UImage*> Img_Stickers {Img_Sticker01, Img_Sticker02, Img_Sticker03, Img_Sticker04, Img_Sticker05};
		for (UImage* Image : Img_Stickers)
		{
			if (Image && Image->IsVisible() && Image->GetIsEnabled() == true)
			{
				FGeometry ImageGeometry = Image->GetCachedGeometry();
				if (ImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					// 이미지 복사본 생성
					DraggedImageCopy = CreateDraggableImageCopy(Image);
					if (DraggedImageCopy)
					{
						bIsDragging = true;
						CurrentImage = DraggedImageCopy;
                        
						// 초기 위치 설정
						FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
						FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(86.5f, 86.5f));

						if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CurrentImage->Slot))
						{
							CanvasSlot->SetSize(FVector2D(193,193));
							CanvasSlot->SetPosition(AdjustedPosition);
							CanvasSlot->SetZOrder(100);
						}
						Img_Copieds.Add(DraggedImageCopy);
						return FReply::Handled().CaptureMouse(this->TakeWidget());
					}
				}
			}
		}
		// 복사본 이미지 클릭 처리
		for(UImage* Copied : Img_Copieds)
		{
			if (Copied && Copied->IsVisible() && Copied->GetIsEnabled() == true)
			{
				FGeometry ImageGeometry = Copied->GetCachedGeometry();
				if (ImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					bIsDragging = true;
					CurrentImage = Copied;
					
					FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
					FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(86.5f));
					
					if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CurrentImage->Slot))
					{
						CanvasSlot->SetSize(FVector2D(193));
						CanvasSlot->SetPosition(AdjustedPosition);
						CanvasSlot->SetZOrder(100);
					}
					return FReply::Handled().CaptureMouse(this->TakeWidget());
				}
			}
		}
	}
	return FReply::Unhandled();
}

FReply UHM_TicketCustom::NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsDragging && CurrentImage)
	{
		// 마우스의 현재 위치를 로컬 좌표로 변환
		FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(86.5f));
		
		// 캔버스 슬롯의 위치 직접 업데이트
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CurrentImage->Slot))
		{
			CanvasSlot->SetPosition(AdjustedPosition);
        }
        
		// 현재 이미지의 Transform 업데이트
		//FWidgetTransform CurrentTransform = CurrentImage->GetRenderTransform();
		//CurrentTransform.Translation = NewPosition;
		//CurrentImage->SetRenderTransform(CurrentTransform);
        
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UHM_TicketCustom::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
	{
		bIsDragging = false;
		
		 if (CurrentImage)
		 {
		 	// 아웃라인 이미지 생성
		 	UImage* OutLineImage = NewObject<UImage>(this, UImage::StaticClass());
		 	if (OutLineImage)
		 	{
		 		// 최종 이미지의 텍스처 설정
		 		UTexture2D* TextureResource = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/Guide.Guide'")));
		 		if (TextureResource)
		 		{
		 			OutLineImage->SetBrushFromTexture(TextureResource);
		 		}
		
		 		// 새로운 이미지의 속성 설정
		 		OutLineImage->SetDesiredSizeOverride(CurrentImage->GetBrush().GetImageSize());
		 		OutLineImage->SetColorAndOpacity(CurrentImage->GetColorAndOpacity());
		 		OutLineImage->SetVisibility(ESlateVisibility::Visible);
		 		OutLineImage->bIsVariable = true;
		 		OutLineImage->SetIsEnabled(true);
		
		 		// 캔버스에 추가
		 		UCanvasPanelSlot* NewCanvasSlot = RootCanvas->AddChildToCanvas(OutLineImage);
		 		if (NewCanvasSlot)
		 		{
		 			// 최종 이미지의 위치와 크기 설정 (원하는 위치에 맞게 조정 가능)
		 			//FVector2D Position = CurrentImage->GetCachedGeometry().GetAbsolutePosition();
		 			FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		 			FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(120));
		 			NewCanvasSlot->SetPosition(AdjustedPosition);
		 			NewCanvasSlot->SetSize(FVector2D(240));
		 			NewCanvasSlot->SetZOrder(100); // ZOrder를 조정하여 다른 이미지 위에 표시
		 			//UE_LOG(LogTemp , Log , TEXT("%d"), NewCanvasSlot->GetZOrder());
		 		}
		 	}
		 }

		CurrentImage = nullptr;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
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
