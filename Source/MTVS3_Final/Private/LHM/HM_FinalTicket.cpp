// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_FinalTicket.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_TicketBG.h"
#include "LHM/HM_TicketCustom.h"
#include "Slate/WidgetRenderer.h"

void UHM_FinalTicket::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_FinalTicket::OnClickedExitButton);
	
	RootCanvas = Cast<UCanvasPanel>(GetRootWidget());
    	if( RootCanvas && Img_FinalTicket && Img_FinalTicketInfo )
    	{
    		// 에디터에서 설정된 위치와 크기 사용
    		UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(Img_FinalTicket->Slot);
    		UCanvasPanelSlot* InfoSlot = Cast<UCanvasPanelSlot>(Img_FinalTicketInfo->Slot);
    		
    		if (BackgroundSlot)
    		{
    			BackgroundSlot->SetSize(FVector2D(888, 504));
    			BackgroundSlot->SetPosition(FVector2D(-220,0));
    			BackgroundSlot->SetAlignment(FVector2d(0.5));
    		}
    
    		if (InfoSlot)
    		{
    			InfoSlot->SetSize(FVector2D(436, 504));
    			InfoSlot->SetPosition(FVector2D(445,0));
    			InfoSlot->SetAlignment(FVector2d(0.5));
    		}
    	}
	
	if (TicketCutomWidget)
	{
		TicketCutomUI = CreateWidget<UHM_TicketCustom>(GetWorld(), TicketCutomWidget);
	}
}

void UHM_FinalTicket::CaptureAndDisplayTicketBackground(UHM_TicketCustom* _TicketCutomUI)
{
	UE_LOG(LogTemp, Log, TEXT("Starting capture process..."));

	this->TicketCutomUI = _TicketCutomUI;
	if(TicketCutomUI == nullptr) return;
	
	if(TicketCutomUI)
	{
		TicketCutomUI->Btn_ResetBackground->SetVisibility(ESlateVisibility::Hidden);
		
		// FWidgetRenderer 생성
		TSharedPtr<FWidgetRenderer> WidgetRenderer = MakeShareable(new FWidgetRenderer(true));
	
		// Render Target 생성 및 크기 설정
		CaptureSize = FVector2D(888.0f, 504.0f); // 기본 크기 설정
		//CaptureSize = FVector2D(1920.0f, 1080.0f); // 기본 크기 설정

		FWidgetTransform WidgetTransform;
		WidgetTransform.Translation = FVector2D(388, 30); // X, Y 좌표
		//WidgetTransform.Translation = FVector2D(0, 0); // X, Y 좌표
		TicketCutomUI->SetRenderTransform(WidgetTransform);
		
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
		RenderTarget->InitCustomFormat(CaptureSize.X, CaptureSize.Y, PF_B8G8R8A8, true);
		RenderTarget->UpdateResource();
	
		// ImgTicketBackgroundWidgetInstance 캡처
		WidgetRenderer->DrawWidget(RenderTarget, TicketCutomUI->TakeWidget(), CaptureSize, 1.0f);

		// Render Target을 UTexture2D로 변환
		UTexture2D* CapturedTexture = ConvertRenderTargetToTexture(this, RenderTarget);
		if (CapturedTexture)
		{
			CapturedTexture->UpdateResource(); // 리소스 업데이트

			// Img_FinalTicket에 캡처된 텍스처 표시
			if (TicketCutomUI && Img_FinalTicket)
			{
				//FSlateBrush Brush;
				//Brush.SetResourceObject(CapturedTexture);
			
				// 최종 티켓 이미지에 반영
				Img_FinalTicket->SetBrushFromTexture(CapturedTexture);
			
				// 서버에 커스텀 티켓 저장 요청
				 AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
				 	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
				 UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
				 if (!GI && !HttpActor3) return;
				 TArray<uint8> ImageData = ConvertTextureToPNG(RenderTarget);
				 HttpActor3->ReqPostSaveCustomTicket(ImageData , HttpActor3->GetStickerIds() , HttpActor3->GetBackgroundId() , GI->GetAccessToken());
				
			}
		}	
	}
}

TArray<uint8> UHM_FinalTicket::ConvertTextureToPNG(UTextureRenderTarget2D* RenderTarget)
{
	TArray<uint8> PNGData;
	if (!RenderTarget) return PNGData;

	// Render Target의 리소스를 가져옴
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	// 픽셀 데이터를 FColor 형식의 배열에 읽어옴
	TArray<FColor> Bitmap;
	RenderTargetResource->ReadPixels(Bitmap);

	// 이미지 래퍼 모듈을 사용해 PNG 형식으로 변환
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.Num() * sizeof(FColor), RenderTarget->SizeX, RenderTarget->SizeY, ERGBFormat::BGRA, 8))
	{
		PNGData = ImageWrapper->GetCompressed(100);  // 압축 레벨 100으로 PNG로 변환
	}

	return PNGData;
}

UTexture2D* UHM_FinalTicket::ConvertRenderTargetToTexture(UObject* WorldContextObject,
                                                          UTextureRenderTarget2D* RenderTarget)
{
	if (!RenderTarget) return nullptr;

	UTexture2D* NewTexture = NewObject<UTexture2D>(WorldContextObject);
	UKismetRenderingLibrary::ConvertRenderTargetToTexture2DEditorOnly(WorldContextObject, RenderTarget, NewTexture);
	
	return NewTexture;
}
