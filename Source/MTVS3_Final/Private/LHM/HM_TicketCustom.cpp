// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TicketCustom.h"
#include "LHM/HM_FinalTicket.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"


void UHM_TicketCustom::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ResetBackground->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetBackgroundButton);
	Btn_ResetTicketImage->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetTicketImageButton);
	Btn_Save->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedSaveButton);
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedExitButton);

	RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	//TicketCanvas = Cast<UCanvasPanel>(GetRootWidget());
	
	bIsDragging = false;
	bIsRenderingAngle = false;
	bIsRenderingScale = false;
	bIsDelete = false;
	bIsBackground = false;
	CurrentImage = nullptr;

	 if (!ScrollBox_Stickers || !VerticalBox_Stickers)
	 {
	 	UE_LOG(LogTemp, Error, TEXT("ScrollBox or VerticalBox is not bound in widget blueprint"));
	 	return;
	 }

	// 스크롤박스 설정
	ScrollBox_Stickers->SetScrollBarVisibility(ESlateVisibility::Visible);
	ScrollBox_Stickers->SetAnimateWheelScrolling(true);
	ScrollBox_Stickers->SetAllowOverscroll(true);
	
}

void UHM_TicketCustom::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);
	
}

void UHM_TicketCustom::SetTextTicketInfo(FString _ConcertName, int32 _Year, int32 _Monthm, int32 _Day, FString _Time, FString _SeatInfo)
{
	Txt_ConcertName->SetText(FText::FromString(_ConcertName));
	Txt_Year->SetText(FText::FromString(FString::FromInt(_Year)));
	Txt_Month->SetText(FText::AsNumber(_Monthm));
	Txt_Day->SetText(FText::AsNumber(_Day));
	Txt_Time->SetText(FText::FromString(_Time));
	Txt_SeatInfo->SetText(FText::FromString(_SeatInfo));
}

FUsedImage UHM_TicketCustom::CreateCompleteImageSet(UImage* SourceImage)
{
	if(!RootCanvas || !SourceImage ) return FUsedImage();
	
	// UOverlay 컨테이너 생성 (이미지 그룹을 관리할 컨테이너)
	UOverlay* ImageGroupOverlay = NewObject<UOverlay>(this, UOverlay::StaticClass());
	if (!ImageGroupOverlay) return FUsedImage();
	
	// RootCanvas에 UOverlay를 추가하여 CanvasPanelSlot을 얻음
	UCanvasPanelSlot* GroupSlotOverlay = Cast<UCanvasPanelSlot>(ImageGroupOverlay->Slot);
	if (GroupSlotOverlay)
	{
		GroupSlotOverlay->SetAlignment(FVector2d(0.5));
	}
	
	UCanvasPanelSlot* GroupSlot = RootCanvas->AddChildToCanvas(ImageGroupOverlay);
	//UCanvasPanelSlot* GroupSlot = TicketCanvas->AddChildToCanvas(ImageGroupOverlay);
	if (!GroupSlot) return FUsedImage();
	
	GroupSlot->SetSize(FVector2D(230));
	GroupSlot->SetAlignment(FVector2d(0.5f));
	GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1); // 필요 시 ZOrder 조정
	
	// 새 이미지 생성
	UImage* CopiedImage = NewObject<UImage>(this, UImage::StaticClass());
	UImage* OutlineImage = NewObject<UImage>(this , UImage::StaticClass());
	UImage* RenderAngleImage = NewObject<UImage>(this , UImage::StaticClass());
	UImage* RenderScaleImage = NewObject<UImage>(this , UImage::StaticClass());
	UImage* RenderDeleteImage = NewObject<UImage>(this , UImage::StaticClass());

	if (CopiedImage && OutlineImage && RenderAngleImage && RenderScaleImage && RenderDeleteImage)
	{
		// 원본 이미지의 속성 복사
		UTexture2D* TextureResource = Cast<UTexture2D>(SourceImage->GetBrush().GetResourceObject());
		if (TextureResource)
		{
			CopiedImage->SetBrushFromTexture(TextureResource);
			CopiedImage->SetDesiredSizeOverride(SourceImage->GetBrush().GetImageSize());
			CopiedImage->SetColorAndOpacity(SourceImage->GetColorAndOpacity());
			CopiedImage->SetVisibility(ESlateVisibility::Visible);
			CopiedImage->bIsVariable = true;
			CopiedImage->SetIsEnabled(true);
		}

		// 아웃라인, 조정버튼 이미지의 텍스처 설정
		UTexture2D* OutlineTexture  = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
		                                                                 TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_StickerOutline.T_StickerOutline'")));
		UTexture2D* AngleTexture  = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
		                                                                 TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_StickerAngle.T_StickerAngle'")));
		UTexture2D* ScaleTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
		                                                                 TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_StickerScale.T_StickerScale'")));
		UTexture2D* DeleteTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass() , nullptr ,
		                                                                 TEXT("/Script/Engine.Texture2D'/Game/LHM/Texture/T_StickerDelete.T_StickerDelete'")));
		if (OutlineTexture && AngleTexture && ScaleTexture && DeleteTexture)
		{
			OutlineImage->SetBrushFromTexture(OutlineTexture);
			RenderAngleImage->SetBrushFromTexture(AngleTexture);
			RenderScaleImage->SetBrushFromTexture(ScaleTexture);
			RenderDeleteImage->SetBrushFromTexture(DeleteTexture);
		}
		// 속성 설정
		OutlineImage->SetDesiredSizeOverride(SourceImage->GetBrush().GetImageSize());
		RenderAngleImage->SetDesiredSizeOverride(FVector2d(45));
		RenderScaleImage->SetDesiredSizeOverride(FVector2d(45));
		RenderDeleteImage->SetDesiredSizeOverride(FVector2d(45));
		OutlineImage->SetVisibility(ESlateVisibility::Hidden);
		RenderAngleImage->SetVisibility(ESlateVisibility::Hidden);
		RenderScaleImage->SetVisibility(ESlateVisibility::Hidden);
		RenderDeleteImage->SetVisibility(ESlateVisibility::Hidden);
		OutlineImage->bIsVariable = true;
		RenderAngleImage->bIsVariable = true;
		RenderScaleImage->bIsVariable = true;
		RenderDeleteImage->bIsVariable = true;
		OutlineImage->SetIsEnabled(true);
		RenderAngleImage->SetIsEnabled(true);
		RenderScaleImage->SetIsEnabled(true);
		RenderDeleteImage->SetIsEnabled(true);

		CopiedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		CopiedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		OutlineImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderAngleImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderScaleImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		RenderDeleteImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		

		// UOverlay에 이미지를 자식으로 추가
		ImageGroupOverlay->AddChildToOverlay(CopiedImage);
		ImageGroupOverlay->AddChildToOverlay(OutlineImage);
		ImageGroupOverlay->AddChildToOverlay(RenderAngleImage);
		ImageGroupOverlay->AddChildToOverlay(RenderScaleImage);
		ImageGroupOverlay->AddChildToOverlay(RenderDeleteImage);
		
		// GroupSlot 설정: 그룹 전체의 위치 및 크기 제어
		UCanvasPanelSlot* CopiedSlot = Cast<UCanvasPanelSlot>(SourceImage->Slot);
		if (CopiedSlot)
		{
			FVector2D Position = CopiedSlot->GetPosition();
			CopiedSlot->SetPosition(Position);
			CopiedSlot->SetAlignment(FVector2d(0.5f));
			CopiedSlot->SetSize(FVector2D(230));
			CopiedSlot->SetZOrder(Img_CopiedImgs.Num() + 1); // 필요 시 ZOrder 조정
			 GroupSlot->SetPosition(Position);
			 GroupSlot->SetSize(FVector2D(230));
			 GroupSlot->SetAlignment(FVector2d(0.5f));
			 GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1); // 필요 시 ZOrder 조정
		}

		// 각 이미지의 개별 위치와 크기 설정 (중앙을 기준으로 조정)
		CopiedImage->SetDesiredSizeOverride(FVector2d(193));
		OutlineImage->SetDesiredSizeOverride(FVector2d(193));
		
		UOverlaySlot* CopiedImgSlot = Cast<UOverlaySlot>(CopiedImage->Slot);
		UOverlaySlot* OutlineImgSlot = Cast<UOverlaySlot>(OutlineImage->Slot);
		UOverlaySlot* AngleImgSlot = Cast<UOverlaySlot>(RenderAngleImage->Slot);
		UOverlaySlot* ScaleImgSlot = Cast<UOverlaySlot>(RenderScaleImage->Slot);
		UOverlaySlot* DeleteImgSlot = Cast<UOverlaySlot>(RenderDeleteImage->Slot);
		if (CopiedImgSlot && OutlineImgSlot && AngleImgSlot && ScaleImgSlot && DeleteImgSlot)
		{
			// Padding 설정
			//CopiedImgSlot->SetPadding(FMargin(0));
			CopiedImgSlot->SetHorizontalAlignment(HAlign_Center);
			CopiedImgSlot->SetVerticalAlignment(VAlign_Center);
			OutlineImgSlot->SetHorizontalAlignment(HAlign_Center);
			OutlineImgSlot->SetVerticalAlignment(VAlign_Center);
			AngleImgSlot->SetHorizontalAlignment(HAlign_Left);
			AngleImgSlot->SetVerticalAlignment(VAlign_Top);
			ScaleImgSlot->SetHorizontalAlignment(HAlign_Right);
			ScaleImgSlot->SetVerticalAlignment(VAlign_Bottom);
			DeleteImgSlot->SetHorizontalAlignment(HAlign_Right);
			DeleteImgSlot->SetVerticalAlignment(VAlign_Top);
		}
		
		CopiedImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		OutlineImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		RenderAngleImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		RenderScaleImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		RenderDeleteImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		
		// Img_CopiedImgs에 추가
		FUsedImage NewImageSet(CopiedImage, OutlineImage, RenderAngleImage, RenderScaleImage, RenderDeleteImage, ImageGroupOverlay, SourceImage);
		Img_CopiedImgs.Add(NewImageSet);
		
		return NewImageSet;
	}
	return FUsedImage();  // 실패 시 기본값 반환
}

void UHM_TicketCustom::SetRenderScale(FUsedImage& ImageSet, const FVector2D& MouseDelta)
{
	if(bIsRenderingScale)
	{
		if(ImageSet.CopiedImage)
		{
			// 마우스 이동 거리의 크기에 따라 Scale 조정
			//float ScaleFactor = 1.0f + (MouseDelta.X * 0.01f);
			float ScaleFactor = FMath::Clamp(1.0f + (MouseDelta.X * 0.005f), 0.5f, 5);
			ImageSet.ImageGroupOverlay->SetRenderScale(FVector2D(ScaleFactor));
		}	
	}
}

void UHM_TicketCustom::SetRenderAngle(FUsedImage& ImageSet, const FVector2D& MouseDelta)
{
	if(bIsRenderingAngle)
	{
		if (ImageSet.CopiedImage)
		{
			// 마우스 이동에 따라 회전 각도 조정
			// 임의 조절 비율 0.5
			float AngleDelta = MouseDelta.X * 0.5f;
			float CurrentImageAngle = ImageSet.ImageGroupOverlay->GetRenderTransformAngle();
			ImageSet.ImageGroupOverlay->SetRenderTransformAngle(CurrentImageAngle + AngleDelta);
		}	
	}
}

void UHM_TicketCustom::DeleteImage(FUsedImage& ImageSet, int32 Index)
{
	if (Img_CopiedImgs.IsValidIndex(Index)) // 인덱스가 유효한지 확인
	{
		ImageSet = Img_CopiedImgs[Index];

		if (bIsDelete)
		{
			if (ImageSet.ImageGroupOverlay)
			{
				// ImageGroupOverlay와 그 하위 위젯들을 UI에서 제거
				ImageSet.ImageGroupOverlay->RemoveFromParent();
			}
			if (ImageSet.OriginImage)
			{
				ImageSet.OriginImage->SetVisibility(ESlateVisibility::Visible);
			}

			// 구조체의 이미지 포인터를 nullptr로 설정하여 이후 접근 방지
			ImageSet.CopiedImage = nullptr;
			ImageSet.Outline = nullptr;
			ImageSet.RenderAngle = nullptr;
			ImageSet.RenderScale = nullptr;
			ImageSet.Delete = nullptr;
			ImageSet.ImageGroupOverlay = nullptr;
			ImageSet.OriginImage = nullptr;

			// 배열에서 해당 인덱스 제거
			Img_CopiedImgs.RemoveAt(Index);
            
			UE_LOG(LogTemp, Log, TEXT("Deleted ImageSet at index: %d"), Index);
		}

		bIsDelete = false;
		CurrentImage = nullptr;
		PreviousImage = nullptr;
	}
}

FReply UHM_TicketCustom::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		for (UImage* Image : StickerImages)
		{
			if (Image && Image->IsVisible() && Image->GetIsEnabled() == true)
			{
				FGeometry CopiedImageGeometry = Image->GetCachedGeometry();
				if (CopiedImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					//OriginImage = Image;
					Image->SetVisibility(ESlateVisibility::Hidden);
					
					// 이미지 복사본 생성 (UOverlay 포함된 ImageSet 생성)
					FUsedImage ImageSet = CreateCompleteImageSet(Image);
					if (ImageSet.CopiedImage && ImageSet.ImageGroupOverlay)
					{
						bIsDragging = true;
						CurrentImage = ImageSet.CopiedImage;

						// 마우스 위치 계산
						FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
						FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(100));

						// UOverlay의 CanvasPanelSlot 위치를 설정
						UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot);
						if (GroupSlot)
						{
							GroupSlot->SetPosition(AdjustedPosition);
							GroupSlot->SetSize(FVector2D(230));  // 그룹 전체 크기 설정
							GroupSlot->SetAlignment(FVector2d(0.5f));
							GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
						}
						return FReply::Handled().CaptureMouse(this->TakeWidget());
					}
				}
			}
		}
		// 복사본 이미지 클릭 처리
		for(const FUsedImage& ImageSet : Img_CopiedImgs)
		{
			UImage* CopiedImg = ImageSet.CopiedImage;
			//UImage* Outline = ImageSet.Outline;
			UImage* Angle = ImageSet.RenderAngle;
			UImage* Scale = ImageSet.RenderScale;
			UImage* Delete = ImageSet.Delete;
			
			// CopiedImage가 클릭되었는지 확인
			if (CopiedImg && CopiedImg->IsVisible() && CopiedImg->GetIsEnabled() == true)
			{
				FGeometry CopiedImageGeometry = CopiedImg->GetCachedGeometry();
				if (CopiedImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					bIsDragging = true;
					PreviousImage = CurrentImage;
					CurrentImage = CopiedImg;
					
					FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
					FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(100));
					// UOverlay의 CanvasPanelSlot 위치를 설정
					UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot);
					if (GroupSlot)
					{
						GroupSlot->SetPosition(AdjustedPosition);
						GroupSlot->SetSize(FVector2D(230));  // 그룹 전체 크기 설정
						GroupSlot->SetAlignment(FVector2d(0.5f));
						GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
					}
					return FReply::Handled().CaptureMouse(this->TakeWidget());
				}
			}

			// RenderAngle이 클릭되었는지 확인
			if (Angle && Angle->IsVisible() && Angle->GetIsEnabled())
			{
				FGeometry AngleGeometry = Angle->GetCachedGeometry();
				if (AngleGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					// RenderAngle이 클릭된 경우 플래그 설정
					bIsRenderingAngle = true;
					CurrentImage = CopiedImg;; // 회전값 조정할 이미지를 설정
					PreviousMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

					return FReply::Handled().CaptureMouse(this->TakeWidget());
				}
			}

			// RenderScale이 클릭되었는지 확인
			if (Scale && Scale->IsVisible() && Scale->GetIsEnabled())
			{
				FGeometry ScaleGeometry = Scale->GetCachedGeometry();
				if (ScaleGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					// RenderAngle이 클릭된 경우 플래그 설정
					bIsRenderingScale = true;
					CurrentImage = CopiedImg;; // 스케일값 조정할 이미지를 설정
					PreviousMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

					return FReply::Handled().CaptureMouse(this->TakeWidget());
				}
			}

			// Delete가 클릭되었는지 확인
			if (Delete && Delete->IsVisible() && Delete->GetIsEnabled())
			{
				FGeometry DeleteGeometry = Delete->GetCachedGeometry();
				if (DeleteGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
				{
					// Delete가 클릭된 경우 플래그 설정
					bIsDelete = true;
					CurrentImage = CopiedImg;; // 삭제할 이미지를 설정
					
					return FReply::Handled().CaptureMouse(this->TakeWidget());
				}
			}
		}
		// 백그라운드 클릭 시
		if (Img_TicketBackground && Img_TicketBackground->IsVisible() && Img_TicketBackground->GetIsEnabled() == true)
		{
			FGeometry BGGeometry = Img_TicketBackground->GetCachedGeometry();
			if (BGGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
			{
				bIsBackground = true;
				CurrentImage = Img_TicketBackground;

				// Img_CopiedImgs 내 모든 요소의 Outline, RenderAngle, RenderScale, Delete 숨기기
				for(FUsedImage& ImageSet : Img_CopiedImgs)
				{
					if (ImageSet.Outline) ImageSet.Outline->SetVisibility(ESlateVisibility::Hidden);
					if (ImageSet.RenderAngle) ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Hidden);
					if (ImageSet.RenderScale) ImageSet.RenderScale->SetVisibility(ESlateVisibility::Hidden);
					if (ImageSet.Delete) ImageSet.Delete->SetVisibility(ESlateVisibility::Hidden);
				}

				// 마우스 캡처 설정
				return FReply::Handled().CaptureMouse(this->TakeWidget());
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
		FVector2D AdjustedPosition = LocalMousePosition - (FVector2D(100));

		// Img_CopiedImgs 배열에서 CurrentImage와 매칭되는 쌍을 찾음
		for (FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.CopiedImage == CurrentImage)
			{
				// UOverlay의 CanvasPanelSlot 위치를 설정
				UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot);
				if (GroupSlot)
				{
					GroupSlot->SetPosition(AdjustedPosition);
					GroupSlot->SetSize(FVector2D(230));  // 그룹 전체 크기 설정
					GroupSlot->SetAlignment(FVector2d(0.5f));
					GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
				}
			}
		}
		return FReply::Handled();
	}
	if (bIsRenderingAngle && CurrentImage)
	{
		FVector2d LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		FVector2D MouseDelta = PreviousMousePosition - LocalMousePosition;
		
		for (FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.CopiedImage == CurrentImage)
			{
				SetRenderAngle(ImageSet, MouseDelta);
				PreviousMousePosition = LocalMousePosition;
				return FReply::Handled();
			}
		}
	}
	if (bIsRenderingScale && CurrentImage)
	{
		FVector2d LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		//FVector2D MouseDelta = PreviousMousePosition - LocalMousePosition;
		FVector2D MouseDelta = LocalMousePosition - PreviousMousePosition;
		
		for (FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.CopiedImage == CurrentImage)
			{
				FVector2D LerpedMousePosition = FMath::Lerp(PreviousMousePosition, MouseDelta, 0.1f);
				SetRenderScale(ImageSet, LerpedMousePosition);
				//PreviousMousePosition = LerpedMousePosition;
				PreviousMousePosition = LocalMousePosition;
				return FReply::Handled();
			}
		}
	}
	if (bIsDelete && CurrentImage)
	{
		for (int32 Index = 0; Index < Img_CopiedImgs.Num(); ++Index) // 인덱스를 추적
		{
			FUsedImage& ImageSet = Img_CopiedImgs[Index];

			if (ImageSet.CopiedImage == CurrentImage)
			{
				DeleteImage(ImageSet, Index); // 인덱스를 파라미터로 넘김
				return FReply::Handled();
			}
		}
	}
	return FReply::Unhandled();
}

FReply UHM_TicketCustom::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = false;
		bIsRenderingAngle = false;
		bIsRenderingScale = false;
		//bIsDelete = false;
		
		for(FUsedImage& ImageSet : Img_CopiedImgs)
		{
			// 이미지가 삭제되지 않았는지 (nullptr가 아닌지) 확인
			if (ImageSet.CopiedImage && ImageSet.Outline && ImageSet.RenderAngle && ImageSet.RenderScale && ImageSet.Delete)
			{
				if (ImageSet.CopiedImage == CurrentImage)
				{
					ImageSet.Outline->SetVisibility(ESlateVisibility::Visible);
					ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Visible);
					ImageSet.RenderScale->SetVisibility(ESlateVisibility::Visible);
					ImageSet.Delete->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					ImageSet.Outline->SetVisibility(ESlateVisibility::Hidden);
					ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Hidden);
					ImageSet.RenderScale->SetVisibility(ESlateVisibility::Hidden);
					ImageSet.Delete->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

void UHM_TicketCustom::SetBackgroundImg(UTexture2D* newTexture)
{
	Img_TicketBackground->SetBrushFromTexture(newTexture);
	UE_LOG(LogTemp , Log , TEXT("SetBackgroundImg"));
}

void UHM_TicketCustom::SetStickersImgs(UTexture2D* Texture, int32 ImageIndex)
{
	if (ImageIndex >= 0 && ImageIndex < StickerImages.Num() && Texture)
    {
        if (UImage* ImageWidget = StickerImages[ImageIndex])
        {
            ImageWidget->SetBrushFromTexture(Texture);
            UE_LOG(LogTemp, Log, TEXT("Set Sticker Image for index: %d"), ImageIndex);
        	
        	ImageWidget->SetVisibility(ESlateVisibility::Visible);
        	ImageWidget->InvalidateLayoutAndVolatility();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid image index or texture: %d"), ImageIndex);
    }
}

void UHM_TicketCustom::InitializeStickerImages(int32 Count)
{
	// 기존 스티커 제거
	if( VerticalBox_Stickers == nullptr )
	{
		UE_LOG(LogTemp, Error, TEXT("VerticalBox is null"));
		return;
	}
	if( VerticalBox_Stickers )
	{
		VerticalBox_Stickers->ClearChildren();
		StickerImages.Empty();
	}
	
	// 새로운 이미지 위젯 생성
	for (int32 i = 0; i < Count; i++)
	{
		UImage* NewImage = CreateImageWidget(i);
		if (NewImage)
		{
			// 이미지를 버티컬박스에 추가
			UVerticalBoxSlot* ImageSlot = VerticalBox_Stickers->AddChildToVerticalBox(NewImage);
			if (ImageSlot)
			{
				// 이미지 슬롯 설정
				//ImageSlot->SetHorizontalAlignment(HAlign_Center);
				ImageSlot->SetVerticalAlignment(VAlign_Center);
			}
            
			StickerImages.Add(NewImage);
			
			// 마지막 이미지가 아닌 경우에만 스페이서 추가
			if (i < Count - 1)
			{
				USpacer* Spacer = CreateSpacerWidget();
				if (Spacer)
				{
					UVerticalBoxSlot* SpacerSlot = VerticalBox_Stickers->AddChildToVerticalBox(Spacer);
					if (SpacerSlot)
					{
						SpacerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic)); // 크기 규칙 설정
					}
				}
			}
		}
	}
	SetVisibility(ESlateVisibility::Visible);
}

UImage* UHM_TicketCustom::CreateImageWidget(int32 Index)
{
	UImage* NewImage = NewObject<UImage>(this, FName(*FString::Printf(TEXT("StickerImage_%d"), Index)));
    if (NewImage)
    {
        // 이미지 기본 설정
        FSlateBrush Brush;
        Brush.DrawAs = ESlateBrushDrawType::Image;
    	Brush.ImageSize = FVector2D(200.0f, 200.0f);
        NewImage->SetBrush(Brush);

    }
    return NewImage;
}

USpacer* UHM_TicketCustom::CreateSpacerWidget()
{
	USpacer* NewSpacer = NewObject<USpacer>(this);
	if (NewSpacer)
	{
		NewSpacer->SetSize(FVector2D(1.0f, 10.0f));
	}
	return NewSpacer;
}

void UHM_TicketCustom::AddSticker(int32 StickerId, UTexture2D* StickerImage)
{
	// StickerDataArray에 추가
	StickerDataArray.Add(FStickerData(StickerId, StickerImage));

	// 이미지 UI 생성
	UImage* NewStickerWidget = CreateImageWidget(StickerDataArray.Num() - 1);
	if (NewStickerWidget && StickerImage)
	{
		NewStickerWidget->SetBrushFromTexture(StickerImage);

		// UI에 추가
		if (VerticalBox_Stickers)
		{
			UVerticalBoxSlot* ImageSlot = VerticalBox_Stickers->AddChildToVerticalBox(NewStickerWidget);
			if (ImageSlot)
			{
				ImageSlot->SetVerticalAlignment(VAlign_Center);
			}
		}
	}
}

void UHM_TicketCustom::OnClickedResetBackgroundButton()
{
	// 배경 이미지 리셋 통신 요청
	auto* GI = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	auto* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if(HttpActor3)
	{
		HttpActor3->ReqPostBackground(GI->GetAccessToken());
	}
}

void UHM_TicketCustom::OnClickedResetTicketImageButton()
{
	// 티켓 이미지 전체 초기화
	for (int32 Index = Img_CopiedImgs.Num() - 1; Index >= 0; --Index) // 인덱스를 추적
	{
		FUsedImage& ImageSet = Img_CopiedImgs[Index];

		// ImageGroupOverlay와 그 하위 위젯들을 UI에서 제거
		ImageSet.ImageGroupOverlay->RemoveFromParent();
		ImageSet.OriginImage->SetVisibility(ESlateVisibility::Visible);
		
		// 구조체의 이미지 포인터를 nullptr로 설정하여 이후 접근 방지
		ImageSet.CopiedImage = nullptr;
		ImageSet.Outline = nullptr;
		ImageSet.RenderAngle = nullptr;
		ImageSet.RenderScale = nullptr;
		ImageSet.Delete = nullptr;
		ImageSet.ImageGroupOverlay = nullptr;
		ImageSet.OriginImage = nullptr;

		// 배열에서 해당 인덱스 제거
		Img_CopiedImgs.RemoveAt(Index);
            
		UE_LOG(LogTemp, Log, TEXT("Deleted ImageSet at index: %d"), Index);
	}
}