// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MainWidget.h"

#include "AssetTypeCategories.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"
#include "LHM/HM_HttpActor2.h" // 통신 테스트용
#include "Kismet/GameplayStatics.h" // 통신 테스트용

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibleCanvas(true);

	Btn_BackMain->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBackMain);

	//// HttpActor2 공연장 입장 통신 테스트용
	//Btn_GetConcertInfo->OnClicked.AddDynamic(this, &UMainWidget::OnClickedGetConcertInfo);
	//Btn_ConcertEntry->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcertEntry);
	//Btn_SeatRegistrationInquiry->OnClicked.AddDynamic(this , &UMainWidget::OnClickedSeatRegistrationInquiry);
	//Btn_GameResult->OnClicked.AddDynamic(this , &UMainWidget::OnClickedGameResult);
}

void UMainWidget::SetVisibleCanvas(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}
	
	else if(!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::SetTextCurrentTime(FString CurrentTime)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_CurrentTime->SetText(FText::FromString(CurrentTime));
	
}

void UMainWidget::OnClickedBackMain()
{
	// 방에서 퇴장하고 싶다.
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->ExitSession();
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("ExitSession"));
	}
}
<<<<<<< HEAD
/*
void UMainWidget::OnClickedGetConcertInfo()
{
    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
    if ( gi )
    {
        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
        if ( HttpActor2 )
        {
            HttpActor2->ReqGetConcertInfo(gi->GetAccessToken());
        }
    }
}

void UMainWidget::OnClickedConcertEntry()
{
    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
    if ( gi )
    {
        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
        if ( HttpActor2 )
        {
            HttpActor2->ReqPostConcertEntry(HttpActor2->GetConcertName() , gi->GetAccessToken());
        }
    }
}

void UMainWidget::OnClickedSeatRegistrationInquiry()
{
    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
    if ( gi )
    {
        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
        if ( HttpActor2 )
        {
            // AvailableSeats 배열을 가져옴
            const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

            // AvailableSeats의 첫 번째 요소가 있는지 확인
            if ( AvailableSeats.Num() > 0 )
            {
                // 통신 테스트용: 첫 번째 좌석 ID를 가져옴
                // 프로토타입 때: 액터의 (좌석번호) 태그를 가져와야 함
                FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
                UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0]: %s") , *FirstAvailableSeatId);

                HttpActor2->ReqPostSeatRegistrationInquiry(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
            }
        }
    }
}

void UMainWidget::OnClickedGameResult()
{
    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
    if ( gi )
    {
        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
        if ( HttpActor2 )
        {
            // AvailableSeats 배열을 가져옴
            const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

            // AvailableSeats의 첫 번째 요소가 있는지 확인
            if ( AvailableSeats.Num() > 0 )
            {
                // 통신 테스트용: 첫 번째 좌석 ID를 가져옴
                // 프로토타입 때: 액터의 (좌석번호) 태그를 가져와야 함
                FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
                UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0]: %s") , *FirstAvailableSeatId);

                HttpActor2->ReqPostGameResult(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
            }
        }
    }
}*/
=======

//void UMainWidget::OnClickedGetConcertInfo()
//{
//    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
//    if ( gi )
//    {
//        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
//            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
//        if ( HttpActor2 )
//        {
//            HttpActor2->ReqGetConcertInfo(gi->GetAccessToken());
//        }
//    }
//}
//
//void UMainWidget::OnClickedConcertEntry()
//{
//    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
//    if ( gi )
//    {
//        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
//            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
//        if ( HttpActor2 )
//        {
//            HttpActor2->ReqPostConcertEntry(HttpActor2->GetConcertName() , gi->GetAccessToken());
//        }
//    }
//}
//
//void UMainWidget::OnClickedSeatRegistrationInquiry()
//{
//    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
//    if ( gi )
//    {
//        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
//            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
//        if ( HttpActor2 )
//        {
//            // AvailableSeats 배열을 가져옴
//            const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();
//
//            // AvailableSeats의 첫 번째 요소가 있는지 확인
//            if ( AvailableSeats.Num() > 0 )
//            {
//                // 통신 테스트용: 첫 번째 좌석 ID를 가져옴
//                // 프로토타입 때: 액터의 (좌석번호) 태그를 가져와야 함
//                FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
//                UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0]: %s") , *FirstAvailableSeatId);
//
//                HttpActor2->ReqPostSeatRegistrationInquiry(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
//            }
//        }
//    }
//}
//
//void UMainWidget::OnClickedGameResult()
//{
//    auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
//    if ( gi )
//    {
//        AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
//            UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
//        if ( HttpActor2 )
//        {
//            // AvailableSeats 배열을 가져옴
//            const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();
//
//            // AvailableSeats의 첫 번째 요소가 있는지 확인
//            if ( AvailableSeats.Num() > 0 )
//            {
//                // 통신 테스트용: 첫 번째 좌석 ID를 가져옴
//                // 프로토타입 때: 액터의 (좌석번호) 태그를 가져와야 함
//                FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
//                UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0]: %s") , *FirstAvailableSeatId);
//
//                HttpActor2->ReqPostGameResult(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
//            }
//        }
//    }
//}
>>>>>>> 87ec297d7e2deda26a5cf248f5dc03f8d6eecced
