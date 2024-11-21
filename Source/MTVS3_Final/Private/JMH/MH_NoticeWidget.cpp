// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_NoticeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_NoticeMessage.h"

void UMH_NoticeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Back_Notice->OnClicked.AddDynamic(this,&UMH_NoticeWidget::CloseBtn_Notice);
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
					// OnMessageClicked 이벤트 바인딩
					MessageBox->OnMessageClicked.AddDynamic(this, &UMH_NoticeWidget::OnMessageSelected);
					Vertical_MessageBox->AddChild(MessageBox);
				}

				// 2. 스페이서 추가
				USpacer* Spacer = NewObject<USpacer>(this);
				if (Spacer)
				{
					Spacer->SetSize(FVector2D(0.0f, 15.0f)); // 세로 방향으로 15px 간격 추가
					Vertical_MessageBox->AddChild(Spacer);
				}
			}
		}
	}
}

void UMH_NoticeWidget::OnMessageSelected(int32 MailId)
{
	UE_LOG(LogTemp, Log, TEXT("Selected MailId: %d"), MailId);

	// 특정 우편함 조회 요청
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if (GI && HttpActor3)
	{
		HttpActor3->ReqGetSpecificMail(MailId, GI->GetAccessToken());
	}
}