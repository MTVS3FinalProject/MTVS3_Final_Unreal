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
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"


void UHM_TicketCustom::OnClickedExitButton()
{
	OnClickedTicketCustomBack.Broadcast();
}

void UHM_TicketCustom::OnClickedSaveButton()
{
	// 티켓 저장할 때 Img_CopiedImgs 내 모든 요소의 Outline, RenderAngle, RenderScale, Delete 숨기기
	for (FUsedImage& ImageSet : Img_CopiedImgs)
	{
		if (ImageSet.Outline) ImageSet.Outline->SetVisibility(ESlateVisibility::Hidden);
		if (ImageSet.RenderAngle) ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Hidden);
		if (ImageSet.RenderScale) ImageSet.RenderScale->SetVisibility(ESlateVisibility::Hidden);
		if (ImageSet.Delete) ImageSet.Delete->SetVisibility(ESlateVisibility::Hidden);
	}
	OnClickedTicketCustomSave.Broadcast();
}

void UHM_TicketCustom::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ResetBackground->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetBackgroundButton);
	Btn_ResetTicketImage->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedResetTicketImageButton);
	Btn_Save->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedSaveButton);
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_TicketCustom::OnClickedExitButton);

	RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
	
	bIsDragging = false;
	bIsRenderingAngle = false;
	bIsRenderingScale = false;
	bIsDelete = false;
	bIsBackground = false;
	CurrentImage = nullptr;

	// 스크롤박스 설정
	if (!ScrollBox_Stickers || !VerticalBox_Stickers) return;
	ScrollBox_Stickers->SetScrollBarVisibility(ESlateVisibility::Visible);
	ScrollBox_Stickers->SetAnimateWheelScrolling(true);
	ScrollBox_Stickers->SetAllowOverscroll(true);

	// 컨트롤 텍스처 로드
	bool bAllTexturesLoaded = true;
    
	if (!OutlineTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("OutlineTexture is not set in the blueprint"));
		bAllTexturesLoaded = false;
	}

	if (!AngleTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("AngleTexture is not set in the blueprint"));
		bAllTexturesLoaded = false;
	}

	if (!ScaleTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("ScaleTexture is not set in the blueprint"));
		bAllTexturesLoaded = false;
	}

	if (!DeleteTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("DeleteTexture is not set in the blueprint"));
		bAllTexturesLoaded = false;
	}

	if (!bAllTexturesLoaded)
	{
		UE_LOG(LogTemp, Error, TEXT("Some textures are not set. Please check the blueprint settings."));
	}
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
    
    // UOverlay 컨테이너 생성
    UOverlay* ImageGroupOverlay = NewObject<UOverlay>(this, UOverlay::StaticClass());
    if (!ImageGroupOverlay) return FUsedImage();
    
    // RootCanvas에 UOverlay를 추가
    UCanvasPanelSlot* GroupSlotOverlay = Cast<UCanvasPanelSlot>(ImageGroupOverlay->Slot);
    if (GroupSlotOverlay)
    {
        GroupSlotOverlay->SetAlignment(FVector2d(0.5));
    }
    
    UCanvasPanelSlot* GroupSlot = RootCanvas->AddChildToCanvas(ImageGroupOverlay);
    if (!GroupSlot) return FUsedImage();
    
    GroupSlot->SetSize(FVector2D(230));
	GroupSlot->SetAlignment(FVector2d(0.5f, 0.5f));
    GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
    
    // 초기 Scale 값 설정
    ImageGroupOverlay->SetRenderScale(FVector2D(1.0f, 1.0f));
    
    // 새 이미지 생성
    UImage* CopiedImage = NewObject<UImage>(this, UImage::StaticClass());
    UImage* OutlineImage = NewObject<UImage>(this, UImage::StaticClass());
    UImage* RenderAngleImage = NewObject<UImage>(this, UImage::StaticClass());
    UImage* RenderScaleImage = NewObject<UImage>(this, UImage::StaticClass());
    UImage* RenderDeleteImage = NewObject<UImage>(this, UImage::StaticClass());
    FVector2d CurrentScale = FVector2d(1.0f, 1.0f);

    if (CopiedImage && OutlineImage && RenderAngleImage && RenderScaleImage && RenderDeleteImage)
    {
        // 원본 이미지의 속성 복사
        UTexture2D* TextureResource = Cast<UTexture2D>(SourceImage->GetBrush().GetResourceObject());
        if (TextureResource)
        {
            CopiedImage->SetBrushFromTexture(TextureResource);
            CopiedTextures.Add(TextureResource);
            CopiedImage->SetDesiredSizeOverride(FVector2D(100.0f, 100.0f));
            CopiedImage->SetRenderScale(FVector2D(1.0f, 1.0f));
            CopiedImage->SetColorAndOpacity(SourceImage->GetColorAndOpacity());
            CopiedImage->SetVisibility(ESlateVisibility::Visible);
            CopiedImage->bIsVariable = true;
            CopiedImage->SetIsEnabled(true);
        }

        // 컨트롤 텍스처 재검증 및 설정
        bool bControlTexturesValid = true;
        if (!OutlineTexture || !AngleTexture || !ScaleTexture || !DeleteTexture)
        {
            if (!OutlineTexture) OutlineTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerOutline"));
            if (!AngleTexture) AngleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerAngle"));
            if (!ScaleTexture) ScaleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerScale"));
            if (!DeleteTexture) DeleteTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerDelete"));
            
            bControlTexturesValid = OutlineTexture && AngleTexture && ScaleTexture && DeleteTexture;
        }

        if (bControlTexturesValid)
        {
            OutlineImage->SetBrushFromTexture(OutlineTexture);
            RenderAngleImage->SetBrushFromTexture(AngleTexture);
            RenderScaleImage->SetBrushFromTexture(ScaleTexture);
            RenderDeleteImage->SetBrushFromTexture(DeleteTexture);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load or apply control textures in CreateCompleteImageSet"));
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

        // UOverlay에 이미지를 자식으로 추가
        ImageGroupOverlay->AddChildToOverlay(CopiedImage);
        ImageGroupOverlay->AddChildToOverlay(OutlineImage);
        ImageGroupOverlay->AddChildToOverlay(RenderAngleImage);
        ImageGroupOverlay->AddChildToOverlay(RenderScaleImage);
        ImageGroupOverlay->AddChildToOverlay(RenderDeleteImage);
        
        // Group Slot 설정
        UCanvasPanelSlot* CopiedSlot = Cast<UCanvasPanelSlot>(SourceImage->Slot);
        if (CopiedSlot)
        {
            FVector2D Position = CopiedSlot->GetPosition();
            CopiedSlot->SetPosition(Position);
            CopiedSlot->SetAlignment(FVector2d(0.5f));
            CopiedSlot->SetSize(FVector2D(230));
            CopiedSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
            GroupSlot->SetPosition(Position);
            GroupSlot->SetSize(FVector2D(230));
            GroupSlot->SetAlignment(FVector2d(0.5f));
            GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
        }

        // 이미지 크기 설정
        CopiedImage->SetDesiredSizeOverride(FVector2d(193));
        OutlineImage->SetDesiredSizeOverride(FVector2d(193));
        
        // Overlay Slot 설정
        UOverlaySlot* CopiedImgSlot = Cast<UOverlaySlot>(CopiedImage->Slot);
        UOverlaySlot* OutlineImgSlot = Cast<UOverlaySlot>(OutlineImage->Slot);
        UOverlaySlot* AngleImgSlot = Cast<UOverlaySlot>(RenderAngleImage->Slot);
        UOverlaySlot* ScaleImgSlot = Cast<UOverlaySlot>(RenderScaleImage->Slot);
        UOverlaySlot* DeleteImgSlot = Cast<UOverlaySlot>(RenderDeleteImage->Slot);
        
        if (CopiedImgSlot && OutlineImgSlot && AngleImgSlot && ScaleImgSlot && DeleteImgSlot)
        {
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
        
        // Transform Pivot 설정
        CopiedImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        OutlineImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        RenderAngleImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        RenderScaleImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        RenderDeleteImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        
        // ImageSet 생성 및 반환
        FUsedImage NewImageSet(CopiedImage, OutlineImage, RenderAngleImage, RenderScaleImage, RenderDeleteImage, ImageGroupOverlay, SourceImage, CurrentScale);
        Img_CopiedImgs.Add(NewImageSet);
        
        return NewImageSet;
    }
    
    return FUsedImage();
}

void UHM_TicketCustom::SetRenderScale(FUsedImage& ImageSet, const FVector2D& MouseDelta)
{
	if(bIsRenderingScale && ImageSet.CopiedImage)
	{
		// 마우스 이동량을 제한
		float AdjustedMouseDelta = MouseDelta.X * 0.001f; // 스케일 변화율을 줄임
		float ScaleFactor = FMath::Clamp(0.001f + AdjustedMouseDelta, 0.5f, 5.0f);
		
		// 초기 Scale을 기준으로 새로운 스케일 계산
		FVector2D NewScale = ImageSet.CurrentScale * ScaleFactor;
		
		// Scale 값 제한
		NewScale.X = FMath::Clamp(NewScale.X, 0.5f, 5.0f);
		NewScale.Y = FMath::Clamp(NewScale.Y, 0.5f, 5.0f);
		
		ImageSet.ImageGroupOverlay->SetRenderScale(NewScale);
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
			float AngleDelta = MouseDelta.X * 0.1f;
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
        // 먼저 존재하는 이미지의 컨트롤 버튼 클릭 체크
        for (const FUsedImage& ImageSet : Img_CopiedImgs)
        {
            // 회전 버튼 클릭
            if (CheckRotationButtonClick(ImageSet, MouseEvent, MyGeometry))
            {
                return FReply::Handled().CaptureMouse(this->TakeWidget());
            }
            
            // 크기 조절 버튼 클릭
            if (CheckScaleButtonClick(ImageSet, MouseEvent, MyGeometry))
            {
                return FReply::Handled().CaptureMouse(this->TakeWidget());
            }
            
            // 삭제 버튼 클릭
            if (CheckDeleteButtonClick(ImageSet, MouseEvent, MyGeometry))
            {
                return FReply::Handled().CaptureMouse(this->TakeWidget());
            }

            // 이미지 자체 클릭
            if (CheckImageClick(ImageSet.CopiedImage, MouseEvent, MyGeometry))
            {
                return FReply::Handled().CaptureMouse(this->TakeWidget());
            }
        }

        // 새로운 스티커 추가 체크
        for (UImage* Image : StickerImages)
        {
            if (Image && Image->IsVisible() && Image->GetIsEnabled())
            {
                FGeometry ImageGeometry = Image->GetCachedGeometry();
                if (ImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
                {
                    CurrentImage = nullptr;
                    Image->SetVisibility(ESlateVisibility::Hidden);
                    
                    FUsedImage ImageSet = CreateCompleteImageSet(Image);
                    if (ImageSet.CopiedImage && ImageSet.ImageGroupOverlay)
                    {
                        bIsDragging = true;
                        CurrentImage = ImageSet.CopiedImage;
                        
                        // 마우스 위치 조정으로 중앙 정렬
                        FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
                    	if (UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot))
                    	{
                    		GroupSlot->SetPosition(LocalMousePosition);  // 오프셋 제거
                    		GroupSlot->SetSize(FVector2D(230));
                    		GroupSlot->SetAlignment(FVector2d(0.5f, 0.5f));  // 중앙 정렬 설정
                    		GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
                        }
                        return FReply::Handled().CaptureMouse(this->TakeWidget());
                    }
                }
            }
        }

        // 마지막으로 배경 클릭 처리
		if (CheckBackgroundClick(MouseEvent))
		{
			return FReply::Handled().CaptureMouse(this->TakeWidget());
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

		// Img_CopiedImgs 배열에서 CurrentImage와 매칭되는 쌍을 찾음
		for (FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.CopiedImage == CurrentImage)
			{
				// UOverlay의 CanvasPanelSlot 위치를 설정
				UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot);
				if (GroupSlot)
				{
					GroupSlot->SetPosition(LocalMousePosition);  // 오프셋 제거
					GroupSlot->SetSize(FVector2D(230));
					GroupSlot->SetAlignment(FVector2d(0.5f, 0.5f));  // 중앙 정렬 설정
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
		FVector2D MouseDelta = LocalMousePosition - PreviousMousePosition;
		
		for (FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.CopiedImage == CurrentImage)
			{
				FVector2D LerpedMousePosition = FMath::Lerp(PreviousMousePosition, MouseDelta, 0.1f);
				SetRenderScale(ImageSet, LerpedMousePosition);
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

		// 텍스처들이 유효한지 먼저 확인
		bool bTexturesValid = true;
		if (!OutlineTexture || !AngleTexture || !ScaleTexture || !DeleteTexture)
		{
			// 텍스처가 유실된 경우 재로드 시도
			OutlineTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerOutline"));
			AngleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerAngle"));
			ScaleTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerScale"));
			DeleteTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/LHM/Texture/T_StickerDelete"));
            
			bTexturesValid = OutlineTexture && AngleTexture && ScaleTexture && DeleteTexture;
			if (!bTexturesValid)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to reload control textures in MouseButtonUp"));
			}
		}

        for(FUsedImage& ImageSet : Img_CopiedImgs)
        {
            // nullptr 체크를 개별적으로 수행
            if (!ImageSet.CopiedImage || !ImageSet.OriginImage || !ImageSet.ImageGroupOverlay)
            {
                continue;
            }

            // 원본 이미지의 브러시와 텍스처 유효성 검사
            FSlateBrush OriginalBrush = ImageSet.OriginImage->GetBrush();
            UTexture2D* OriginalTexture = Cast<UTexture2D>(OriginalBrush.GetResourceObject());
            if (!OriginalTexture)
            {
                UE_LOG(LogTemp, Warning, TEXT("Original texture is invalid"));
                continue;
            }

            if (ImageSet.CopiedImage == CurrentImage)
            {
                // 원본 이미지의 브러시를 안전하게 복사
                FSlateBrush NewBrush = ImageSet.OriginImage->GetBrush();
                if (NewBrush.GetResourceObject())
                {
                    ImageSet.CopiedImage->SetBrush(NewBrush);
                }

                // 컨트롤 버튼들의 텍스처와 가시성 설정
                if (bTexturesValid)
                {
                    if (ImageSet.Outline)
                    {
                        ImageSet.Outline->SetBrushFromTexture(OutlineTexture);
                        ImageSet.Outline->SetVisibility(ESlateVisibility::Visible);
                    }
                    if (ImageSet.RenderAngle)
                    {
                        ImageSet.RenderAngle->SetBrushFromTexture(AngleTexture);
                        ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Visible);
                    }
                    if (ImageSet.RenderScale)
                    {
                        ImageSet.RenderScale->SetBrushFromTexture(ScaleTexture);
                        ImageSet.RenderScale->SetVisibility(ESlateVisibility::Visible);
                    }
                    if (ImageSet.Delete)
                    {
                        ImageSet.Delete->SetBrushFromTexture(DeleteTexture);
                        ImageSet.Delete->SetVisibility(ESlateVisibility::Visible);
                    }
                }
            }
            else
            {
                // 현재 선택되지 않은 이미지의 컨트롤은 숨김
                if (ImageSet.Outline) ImageSet.Outline->SetVisibility(ESlateVisibility::Hidden);
                if (ImageSet.RenderAngle) ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Hidden);
                if (ImageSet.RenderScale) ImageSet.RenderScale->SetVisibility(ESlateVisibility::Hidden);
                if (ImageSet.Delete) ImageSet.Delete->SetVisibility(ESlateVisibility::Hidden);
            }

            // 스케일 조정 중이었다면 최종 스케일 값 저장
            if (bIsRenderingScale && CurrentImage && ImageSet.CopiedImage == CurrentImage)
            {
                ImageSet.CurrentScale = ImageSet.ImageGroupOverlay->GetRenderTransform().Scale;
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

bool UHM_TicketCustom::CheckImageClick(UImage* Image, const FPointerEvent& MouseEvent, const FGeometry& MyGeometry)
{
	if (Image && Image->IsVisible() && Image->GetIsEnabled())
	{
		FGeometry ImageGeometry = Image->GetCachedGeometry();
		if (ImageGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsDragging = true;
			PreviousImage = CurrentImage;
			CurrentImage = Image;
                
			FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
			SetImagePosition(LocalMousePosition);
			return true;
		}
	}
	return false;
}

void UHM_TicketCustom::SetImagePosition(const FVector2D& LocalMousePosition)
{
	for (const FUsedImage& ImageSet : Img_CopiedImgs)
	{
		if (ImageSet.CopiedImage == CurrentImage)
		{
			if (UCanvasPanelSlot* GroupSlot = Cast<UCanvasPanelSlot>(ImageSet.ImageGroupOverlay->Slot))
			{
				GroupSlot->SetPosition(LocalMousePosition);  // 오프셋 제거
				GroupSlot->SetSize(FVector2D(230));
				GroupSlot->SetAlignment(FVector2d(0.5f, 0.5f));  // 중앙 정렬 설정
				GroupSlot->SetZOrder(Img_CopiedImgs.Num() + 1);
			}
			break;
		}
	}
}

bool UHM_TicketCustom::CheckRotationButtonClick(const FUsedImage& ImageSet, const FPointerEvent& MouseEvent,
	const FGeometry& MyGeometry)
{
	if (ImageSet.RenderAngle && ImageSet.RenderAngle->IsVisible() && ImageSet.RenderAngle->GetIsEnabled())
	{
		FGeometry AngleGeometry = ImageSet.RenderAngle->GetCachedGeometry();
		if (AngleGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsRenderingAngle = true;
			CurrentImage = ImageSet.CopiedImage;
			PreviousMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
			return true;
		}
	}
	return false;
}

bool UHM_TicketCustom::CheckScaleButtonClick(const FUsedImage& ImageSet, const FPointerEvent& MouseEvent,
	const FGeometry& MyGeometry)
{
	if (ImageSet.RenderScale && ImageSet.RenderScale->IsVisible() && ImageSet.RenderScale->GetIsEnabled())
	{
		FGeometry ScaleGeometry = ImageSet.RenderScale->GetCachedGeometry();
		if (ScaleGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsRenderingScale = true;
			CurrentImage = ImageSet.CopiedImage;
			PreviousMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
            
			// CurrentScale 초기화
			for (FUsedImage& ImgSet : Img_CopiedImgs)
			{
				if (ImgSet.CopiedImage == CurrentImage)
				{
					ImgSet.CurrentScale = ImgSet.ImageGroupOverlay->GetRenderTransform().Scale;
					break;
				}
			}
			return true;
		}
	}
	return false;
}

bool UHM_TicketCustom::CheckDeleteButtonClick(const FUsedImage& ImageSet, const FPointerEvent& MouseEvent,
	const FGeometry& MyGeometry)
{
	if (ImageSet.Delete && ImageSet.Delete->IsVisible() && ImageSet.Delete->GetIsEnabled())
	{
		FGeometry DeleteGeometry = ImageSet.Delete->GetCachedGeometry();
		if (DeleteGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsDelete = true;
			CurrentImage = ImageSet.CopiedImage;
			return true;
		}
	}
	return false;
}

bool UHM_TicketCustom::CheckBackgroundClick(const FPointerEvent& MouseEvent)
{
	bool bClicked = false;
	if (Img_TicketBackground && Img_TicketBackground->IsVisible() && Img_TicketBackground->GetIsEnabled())
	{
		FGeometry BGGeometry = Img_TicketBackground->GetCachedGeometry();
		if (BGGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsBackground = true;
			CurrentImage = Img_TicketBackground;
			bClicked = true;
		}
	}
    
	if (Img_TicketInfo && Img_TicketInfo->IsVisible() && Img_TicketInfo->GetIsEnabled())
	{
		FGeometry InfoGeometry = Img_TicketInfo->GetCachedGeometry();
		if (InfoGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			bIsBackground = true;
			CurrentImage = Img_TicketInfo;
			bClicked = true;
		}
	}

	if (bClicked)
	{
		// 모든 컨트롤 버튼 숨기기
		for(FUsedImage& ImageSet : Img_CopiedImgs)
		{
			if (ImageSet.Outline) ImageSet.Outline->SetVisibility(ESlateVisibility::Hidden);
			if (ImageSet.RenderAngle) ImageSet.RenderAngle->SetVisibility(ESlateVisibility::Hidden);
			if (ImageSet.RenderScale) ImageSet.RenderScale->SetVisibility(ESlateVisibility::Hidden);
			if (ImageSet.Delete) ImageSet.Delete->SetVisibility(ESlateVisibility::Hidden);
		}
	}
    
	return bClicked;
}
