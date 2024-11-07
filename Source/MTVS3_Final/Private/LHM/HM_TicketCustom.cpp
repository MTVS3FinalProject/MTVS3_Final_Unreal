// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketCustom.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"


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
	bIsRendering = false;
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
		CanvasSlot->SetZOrder(101);
	}
	return NewImage;
}

std::pair<UImage*, UImage*> UHM_TicketCustom::CreateOutlineImageCopy(UImage* SourceImage)
{
	// 아웃라인, 조정버튼 이미지 생성
	UImage* OutLineImage = NewObject<UImage>(this , UImage::StaticClass());
	UImage* RenderBtnImage = NewObject<UImage>(this , UImage::StaticClass());
	if (OutLineImage && RenderBtnImage)
	{
		// 아웃라인, 조정버튼 이미지의 텍스처 설정
		UTexture2D* TextureResource1 = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
		                                                                TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_Guide.T_Guide'")));
		UTexture2D* TextureResource2 = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
																		TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_Render.T_Render'")));
		if (TextureResource1, TextureResource2)
		{
			OutLineImage->SetBrushFromTexture(TextureResource1);
			RenderBtnImage->SetBrushFromTexture(TextureResource2);
		}

		// 속성 설정
		OutLineImage->SetDesiredSizeOverride(SourceImage->GetBrush().GetImageSize());
		OutLineImage->SetColorAndOpacity(SourceImage->GetColorAndOpacity());
		OutLineImage->SetVisibility(ESlateVisibility::Hidden);
		OutLineImage->bIsVariable = true;
		OutLineImage->SetIsEnabled(true);
		
		RenderBtnImage->SetDesiredSizeOverride(SourceImage->GetBrush().GetImageSize());
		RenderBtnImage->SetColorAndOpacity(SourceImage->GetColorAndOpacity());
		RenderBtnImage->SetVisibility(ESlateVisibility::Hidden);
		RenderBtnImage->bIsVariable = true;
		RenderBtnImage->SetIsEnabled(true);

		if (UCanvasPanelSlot* CurrentImageSlot = Cast<UCanvasPanelSlot>(CurrentImage->Slot))
		{
			// OutLineImage를 RootCanvas에 추가
			UCanvasPanelSlot* NewOutLineSlot = RootCanvas->AddChildToCanvas(OutLineImage);
			UCanvasPanelSlot* NewRenderBtnSlot = RootCanvas->AddChildToCanvas(RenderBtnImage);
			if (NewOutLineSlot && NewRenderBtnSlot)
			{
				// CurrentImage의 위치와 크기를 NewCanvasSlot에 적용
				NewOutLineSlot->SetPosition(CurrentImageSlot->GetPosition());
				NewOutLineSlot->SetSize(CurrentImageSlot->GetSize());
				NewOutLineSlot->SetZOrder(102); // ZOrder 설정으로 다른 이미지 위에 표시
				
				NewRenderBtnSlot->SetPosition(CurrentImageSlot->GetPosition()+ FVector2D(170));
				NewRenderBtnSlot->SetSize(FVector2D(35));
				NewRenderBtnSlot->SetZOrder(103);
			}
		}
		Img_CopiedPairs.Add(FImagePair(CurrentImage, OutLineImage, RenderBtnImage));

		// 복사된 이미지 배열 인덱스와 이름 출력
		if (CurrentImage && OutLineImage && RenderBtnImage)
		{
			// OriginalImage와 OutlinedImage의 이름과 인덱스 로그 출력
			UE_LOG(LogTemp, Log, TEXT("Img_CopiedPairs[%d]: CopiedImage = %s, OutlinedImage = %s, RenderBtnImage = %s"),
				   Img_CopiedPairs.Num() - 1,
				   *CurrentImage->GetName(),
				   *OutLineImage->GetName(),
				   *RenderBtnImage->GetName());
		}
	}
	return std::make_pair(OutLineImage, RenderBtnImage);
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
	float CurrentAngle = Image->GetRenderTransformAngle();
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

						// 아웃라인 생성
						if (CurrentImage)
						{
							CreateOutlineImageCopy(CurrentImage);
						}
						return FReply::Handled().CaptureMouse(this->TakeWidget());
					}
				}
			}
		}
		// 복사본 이미지 클릭 처리
		for(const FImagePair& ImagePair : Img_CopiedPairs)
		{
			UImage* Copied = ImagePair.CopiedImage;
			UImage* Outline = ImagePair.OutlineImage;
			UImage* RenderBtn = ImagePair.RenderBtnImage;
			
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
						CanvasSlot->SetZOrder(104);
						
						if(UCanvasPanelSlot* OutlineSlot = Cast<UCanvasPanelSlot>(Outline->Slot))
						{
							OutlineSlot->SetSize(FVector2D(193));
							OutlineSlot->SetPosition(AdjustedPosition);
							OutlineSlot->SetZOrder(105);
						}
						if(UCanvasPanelSlot* RenderBtnSlot = Cast<UCanvasPanelSlot>(RenderBtn->Slot))
						{
							RenderBtnSlot->SetSize(FVector2D(35));
							RenderBtnSlot->SetPosition(AdjustedPosition + FVector2D(170));
							RenderBtnSlot->SetZOrder(106);
							UE_LOG(LogTemp , Log , TEXT("RenderBtn X :%f, Y:%f"),RenderBtnSlot->GetPosition().X, RenderBtnSlot->GetPosition().Y);
						}
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

		// Img_CopiedPairs 배열에서 CurrentImage와 매칭되는 쌍을 찾음
		for (FImagePair& ImagePair : Img_CopiedPairs)
		{
			if (ImagePair.CopiedImage == CurrentImage)
			{
				// 캔버스 슬롯의 위치 직접 업데이트
				UCanvasPanelSlot* CopiedSlot = Cast<UCanvasPanelSlot>(ImagePair.CopiedImage->Slot);
				UCanvasPanelSlot* OutlineSlot = Cast<UCanvasPanelSlot>(ImagePair.OutlineImage->Slot);
				UCanvasPanelSlot* RenderBtnSlot = Cast<UCanvasPanelSlot>(ImagePair.RenderBtnImage->Slot);
				if (CopiedSlot && OutlineSlot && RenderBtnSlot)
				{
					CopiedSlot->SetPosition(AdjustedPosition);
					OutlineSlot->SetPosition(AdjustedPosition);
					RenderBtnSlot->SetPosition(AdjustedPosition+ FVector2D(170));
				}
			}
		}

		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UHM_TicketCustom::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
	{
		bIsDragging = false;

		for(FImagePair& ImagePair : Img_CopiedPairs)
		{
			if (ImagePair.CopiedImage == CurrentImage)
			{
				ImagePair.OutlineImage->SetVisibility(ESlateVisibility::Visible);
				ImagePair.RenderBtnImage->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				ImagePair.OutlineImage->SetVisibility(ESlateVisibility::Hidden);
				ImagePair.RenderBtnImage->SetVisibility(ESlateVisibility::Hidden);
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
