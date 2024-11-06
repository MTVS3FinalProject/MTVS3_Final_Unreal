// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketCustom.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UHM_TicketCustom::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ResetSticker01->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetSticker01Button);
	Btn_ResetSticker02->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetSticker02Button);
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

	// 고유 ID 생성 (예: "CopiedImage_1", "CopiedImage_2" 등)
	//FString ImageID = FString::Printf(TEXT("CopiedImage_%d"), CopiedImages.Num() + 1);
	//CopiedImages.Add(NewImage, ImageID); // 맵에 이미지와 ID 매핑

	// 클릭 이벤트 바인딩
	//NewImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnCopiedImageClicked"));
	
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

FReply UHM_TicketCustom::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		TArray<UImage*> Images = { Img_Sticker01, Img_Sticker02, Img_Sticker03, Img_Sticker04, Img_Sticker05 };
		for (UImage* Image : Images)
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
                        
						FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
						
						// 초기 위치 설정
						if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(DraggedImageCopy->Slot))
						{
							CanvasSlot->SetSize(FVector2D(193,193));
							CanvasSlot->SetPosition(LocalMousePosition);
						}
						return FReply::Handled().CaptureMouse(this->TakeWidget());
					}
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
        
		// 캔버스 슬롯의 위치 직접 업데이트
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CurrentImage->Slot))
		{
			CanvasSlot->SetPosition(LocalMousePosition);
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
		CurrentImage = nullptr;
		return FReply::Handled();
	}
	return FReply::Unhandled().ReleaseMouseCapture();
}

void UHM_TicketCustom::OnCopiedImageClicked()
{
	// ClickedImage = Cast<UImage>(GetOwningPlayer()->GetMousePosition()); // 클릭된 이미지 저장
	// if (ClickedImage)
	// {
	// 	// 추가 작업 수행
	// 	if (FString* ImageID = CopiedImages.Find(ClickedImage)) // 클릭된 이미지의 ID 가져오기
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("Clicked image ID: %s"), **ImageID);
	//
	// 		// 특정 ID와 일치하는 경우 추가 작업 수행
	// 		if (*ImageID == TEXT("CopiedImage_1")) // 첫 번째 이미지일 경우
	// 		{
	// 			UE_LOG(LogTemp, Log, TEXT("Special action for CopiedImage_1"));
	//
	// 			// ZOrder를 가장 위로 설정
	// 			if (UCanvasPanelSlot* ClickedSlot = Cast<UCanvasPanelSlot>(ClickedImage->Slot))
	// 			{
	// 				ClickedSlot->SetZOrder(100);
	// 			}
	// 		}
	// 	}
	// }
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
