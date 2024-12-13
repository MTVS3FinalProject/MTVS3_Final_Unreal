// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_NoticeWidget.h"

#include "HttpModule.h"
#include "ImageUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "HJ/TTGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_NoticeMessage.h"

void UMH_NoticeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Back_Notice->OnClicked.AddDynamic(this , &UMH_NoticeWidget::CloseBtn_Notice);
	Btn_Back_Content->OnClicked.AddDynamic(this , &UMH_NoticeWidget::CloseBtn_Content);
	Btn_Payment->OnClicked.AddDynamic(this , &UMH_NoticeWidget::CloseBtn_Content);
	Btn_Payment->OnClicked.AddDynamic(this, &UMH_NoticeWidget::Payment_Postpone);
}

void UMH_NoticeWidget::InitializeMessageTabs()
{
	//데이터들이 저장될 HttpActor3에서 정보 TArray로 받아오기
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if (!HttpActor3)
	{
		return;
	}
	if (HttpActor3)
	{
		// 받은 데이터가 비어 있지 않은지 확인
		const TArray<FMails>& Mails = HttpActor3->GetMails();

		if (Mails.Num() > 0)
		{
			Vertical_MessageBox->ClearChildren();

			for (const FMails& Messageinfo : Mails)
			{
				// 1. 메세지 위젯 생성
				UHM_NoticeMessage* MessageBox = CreateWidget<UHM_NoticeMessage>(this , NoticeMessageFac);
				if (MessageBox)
				{
					MessageBox->Text_Message->SetText(FText::FromString(Messageinfo.subject));
					MessageBox->SetMailId(Messageinfo.mailId);
					MessageBox->SetMailCategory(Messageinfo.mailCategory);
					// OnMessageClicked 이벤트 바인딩
					MessageBox->OnMessageClicked.AddDynamic(this , &UMH_NoticeWidget::OnMessageSelected);
					Vertical_MessageBox->AddChild(MessageBox);
				}

				// 2. 스페이서 추가
				USpacer* Spacer = NewObject<USpacer>(this);
				if (Spacer)
				{
					Spacer->SetSize(FVector2D(0.0f , 15.0f)); // 세로 방향으로 15px 간격 추가
					Vertical_MessageBox->AddChild(Spacer);
				}
			}
		}
	}
}

void UMH_NoticeWidget::OnMessageSelected(int32 MailId)
{
	UE_LOG(LogTemp , Log , TEXT("Selected MailId: %d") , MailId);

	// 특정 우편함 조회 요청
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if (GI && HttpActor3)
	{
		// MailCategory 값을 가져오는 추가 로직
		FString MailCategory = TEXT(""); // 초기값 설정
		const TArray<FMails>& Mails = HttpActor3->GetMails();
		for (const FMails& Mail : Mails)
		{
			if (Mail.mailId == MailId)
			{
				MailCategory = Mail.mailCategory;
				break;
			}
		}
		
		if (MailCategory == TEXT("GENERAL"))
		{
			HttpActor3->ReqGetSpecificMail(MailId , GI->GetAccessToken());
		}
		else if (MailCategory == TEXT("POSTPONE"))
		{
			HttpActor3->ReqGetSpecificMail(MailId , GI->GetAccessToken());
			HttpActor3->ReqGetPostponePaymentSeatMail(MailId, GI->GetAccessToken());
			Btn_Payment->SetVisibility(ESlateVisibility::Visible);
		}
		else if (MailCategory == TEXT("PUZZLE"))
		{
			HttpActor3->ReqGetPuzzleMail(MailId, GI->GetAccessToken());
		}
		else // RECEIPT, CANCEL, RESERVE
		{
			return;
		}
	}
}

void UMH_NoticeWidget::OnMailDetailReceived(bool bIsPuzzle, FString Subject , FString Content)
{
	if (!Text_Subject || !Text_Content) return;
	if (bIsPuzzle)
	{
		Text_Subject->SetText(FText::FromString(TEXT("퍼즐 이벤트 종료")));
		Text_Content_Puzzle->SetText(FText::FromString(Content));
	}
	else
	{
		Text_Subject->SetText(FText::FromString(Subject));
		Text_Content->SetText(FText::FromString(Content));
		Canvas_General->SetVisibility(ESlateVisibility::Visible);
	}
	
	Canvas_content->SetVisibility(ESlateVisibility::Visible);
	Vertical_MessageBox->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_NoticeWidget::OnPuzzleTitleStickerReceived(int32 Rank, const FTitles& TitleInfo, const FStickers& StickerInfo)
{
	if(Text_Rank && Text_Nickname && Img_Title)
	{
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		Text_Nickname->SetText(FText::FromString(GI->GetNickname()));
		
		FLinearColor LinearColor1 = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);
		FLinearColor LinearColor2 = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
		FLinearColor LinearColor3 = FLinearColor(0.6f, 0.2f, 0.1f, 1.0f);
		FLinearColor LinearColorOutline1 = FLinearColor(0.16f, 0.02f, 0.0f, 1.0f);
		FLinearColor LinearColorOutline2 = FLinearColor(0.03f, 0.03f, 0.03f, 1.0f);
		FLinearColor LinearColorOutline3 = FLinearColor(0.0417f, 0.013f, 0.0065f, 1.0f);
	
		if(Rank == 1)
		{
			FSlateColor SlateColor = FSlateColor(LinearColor1);
			Img_Title->SetBrushTintColor(SlateColor);

			// Text_Rank 폰트, 아웃라인 컬러 변경
			FSlateFontInfo FontInfo = Text_Rank->GetFont();
			FontInfo.OutlineSettings.OutlineColor = LinearColorOutline1;
			Text_Rank->SetFont(FontInfo);
			Text_Rank->SetColorAndOpacity(LinearColor1);
			Text_Rank->SetText(FText::FromString(TEXT("1등")));
		}
		else if(Rank == 2)
		{
			FSlateColor SlateColor = FSlateColor(LinearColor2);
			Img_Title->SetBrushTintColor(SlateColor);
			
			// Text_Rank 폰트, 아웃라인 컬러 변경
			FSlateFontInfo FontInfo = Text_Rank->GetFont();
			FontInfo.OutlineSettings.OutlineColor = LinearColorOutline2;
			Text_Rank->SetFont(FontInfo);
			Text_Rank->SetColorAndOpacity(LinearColor2);
			Text_Rank->SetText(FText::FromString(TEXT("2등")));
		}
		else if(Rank == 3)
		{
			FSlateColor SlateColor = FSlateColor(LinearColor3);
			Img_Title->SetBrushTintColor(SlateColor);
			
			// Text_Rank 폰트, 아웃라인 컬러 변경
			FSlateFontInfo FontInfo = Text_Rank->GetFont();
			FontInfo.OutlineSettings.OutlineColor = LinearColorOutline3;
			Text_Rank->SetFont(FontInfo);
			Text_Rank->SetColorAndOpacity(LinearColor3);
			Text_Rank->SetText(FText::FromString(TEXT("3등")));
		}
	}
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
	ImageRequest->SetURL(StickerInfo.stickerImage);
	ImageRequest->SetVerb(TEXT("GET"));

	ImageRequest->OnProcessRequestComplete().BindLambda(
		[this, StickerInfo](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("ImageRequest failed!"));
				return;
			}
        
			TArray<uint8> ImageData = Response->GetContent();
			if (ImageData.Num() == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Response returned no content."));
				return;
			}
			
			UTexture2D* StickerTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
			if (Img_Sticker && StickerTexture)
			{
				Img_Sticker->SetBrushFromTexture(StickerTexture);
			}
		});
	// HTTP 요청 시작
	ImageRequest->ProcessRequest();

	if (Txt_TitleRarity && Txt_TitleName && Txt_StickerRarity && Txt_StickerName)
	{
		Txt_TitleRarity->SetText(FText::FromString(TitleInfo.titleRarity));
		Txt_TitleName->SetText(FText::FromString(TitleInfo.titleName));
		Txt_StickerRarity->SetText(FText::FromString(StickerInfo.stickerRarity));
		Txt_StickerName->SetText(FText::FromString(StickerInfo.stickerName));
	}

	Canvas_puzzle->SetVisibility(ESlateVisibility::Visible);
}

void UMH_NoticeWidget::CloseBtn_Content()
{
	Canvas_content->SetVisibility(ESlateVisibility::Hidden);
	Canvas_General->SetVisibility(ESlateVisibility::Hidden);
	Canvas_puzzle->SetVisibility(ESlateVisibility::Hidden);
	Btn_Payment->SetVisibility(ESlateVisibility::Hidden);
	Vertical_MessageBox->SetVisibility(ESlateVisibility::Visible);
}
