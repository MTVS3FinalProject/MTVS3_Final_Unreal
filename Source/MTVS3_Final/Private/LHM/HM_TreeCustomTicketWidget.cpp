// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TreeCustomTicketWidget.h"

#include "HttpModule.h"
#include "ImageUtils.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "HJ/TTGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_TreeTicketBoxWidget.h"

void UHM_TreeCustomTicketWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Btn_Yes->OnClicked.AddDynamic(this , &UHM_TreeCustomTicketWidget::OnClickedYesBtn);
	Btn_No->OnClicked.AddDynamic(this , &UHM_TreeCustomTicketWidget::OnClickedNoBtn);
	Btn_Back->OnClicked.AddDynamic(this , &UHM_TreeCustomTicketWidget::OnClickedBackBtn);
}

void UHM_TreeCustomTicketWidget::InitializeTicketTabs(const TMap<int32, FString>& TicketMap)
{
	// 기존 위젯 초기화
	Hori_Box_Tickets->ClearChildren();
	
	for (const TPair<int32, FString>& TicketPair : TicketMap)
	{
		UHM_TreeTicketBoxWidget* ItemBox_Ticket = CreateWidget<UHM_TreeTicketBoxWidget>(this , TicketItemBoxFac);
		if (ItemBox_Ticket)
		{
			// URL에서 이미지 다운로드
			TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
			ImageRequest->SetURL(TicketPair.Value);
			ImageRequest->SetVerb(TEXT("GET"));

			// 다운로드 완료 시 콜백 설정
			ImageRequest->OnProcessRequestComplete().BindLambda(
				[ItemBox_Ticket](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
				{
					if (bWasSuccessful && Response.IsValid())
					{
						// 이미지 데이터 가져오기
						TArray<uint8> ImageData = Response->GetContent();

						// 텍스처로 변환
						UTexture2D* StickerTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
						if (StickerTexture && ItemBox_Ticket->Img_Ticket)
						{
							// 이미지 위젯에 텍스처 적용
							ItemBox_Ticket->Img_Ticket->SetBrushFromTexture(StickerTexture);
							UE_LOG(LogTemp , Log , TEXT("Ticket image set successfully"));
						}
						else
						{
							UE_LOG(LogTemp , Warning ,
								   TEXT("Failed to create TicketTexture or Img_Item_Ticket is null"));
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("Failed to download Ticket image from URL: %s") ,
							   *Request->GetURL());
					}
				});
			ImageRequest->ProcessRequest();
			ItemBox_Ticket->TicketId = TicketPair.Key; // 티켓 ID 저장
			// OnTicketClicked 이벤트 바인딩
			ItemBox_Ticket->OnTicketClicked.AddDynamic(this , &UHM_TreeCustomTicketWidget::OnTicketSelected);
			Hori_Box_Tickets->AddChild(ItemBox_Ticket);
		}
	}
}

void UHM_TreeCustomTicketWidget::OnClickedBackBtn()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}

void UHM_TreeCustomTicketWidget::OnClickedYesBtn()
{
	auto* GI = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	auto* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if(HttpActor3)
	{
		// 티켓아이디
		HttpActor3->ReqPostHangingTicketFromTree(CurrentSelectedTicketId,GI->GetAccessToken());
		Can_Choose->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHM_TreeCustomTicketWidget::OnClickedNoBtn()
{
	Can_Choose->SetVisibility(ESlateVisibility::Hidden);
}

void UHM_TreeCustomTicketWidget::OnTicketSelected(int32 ClickedTicketId)
{
	Can_Choose->SetVisibility(ESlateVisibility::Visible);
	CurrentSelectedTicketId = ClickedTicketId;
}