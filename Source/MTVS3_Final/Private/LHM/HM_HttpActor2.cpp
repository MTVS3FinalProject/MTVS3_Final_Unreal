// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"
#include "JMH/MH_TicketingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "LHM/TTPlayerController.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"

// Sets default values
AHM_HttpActor2::AHM_HttpActor2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHM_HttpActor2::BeginPlay()
{
	Super::BeginPlay();
	
	if ( UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("TTHallMap") )
	{
		// 특정 레벨일 때 실행할 코드
		UE_LOG(LogTemp , Warning , TEXT("현재 레벨은 TTHallMap입니다."));

		TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
		if ( TicketingUI )
		{
			TicketingUI->AddToViewport();
		}

		auto* pc = UGameplayStatics::GetPlayerController(this , 0);
		if ( !pc ) return;
		pc->SetShowMouseCursor(true);
		pc->SetInputMode(FInputModeGameAndUI());
	}
}

// Called every frame
void AHM_HttpActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_HttpActor2::ReqGetSeatRegistrationCount(const FString& SeatId , const FString& AccessToken)
{

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/seats/seatId/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetSeatRegistrationCount);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResGetSeatRegistrationCount(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200 )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 응답에서 접수한 인원 수 가져오기
		int32 RegisteredCount = FCString::Atoi(*Response->GetContentAsString());

		if ( RegisteredCount > 0 ) // 인원 수가 0보다 클 때만 확률 계산
		{
			// 당첨 확률 계산
			float WinningProbability = (1.0f / RegisteredCount) * 100.0f;

			// UI에 당첨 확률 업데이트
			if ( TicketingUI )
			{
				TicketingUI->SetTextCompetitionRate(WinningProbability); // 예시 함수
			}
			UE_LOG(LogTemp , Log , TEXT("Winning Probability: %.2f%%") , WinningProbability); // 로그로 출력
		}
		else
		{
			if ( TicketingUI )
			{
				TicketingUI->SetTextCompetitionRate(0.0f); // 0명일 경우 0%로 설정
			}
			UE_LOG(LogTemp , Warning , TEXT("No registered players found."));
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to get seat registration count: %s") , *Response->GetContentAsString());
	}
}

void AHM_HttpActor2::ReqPostRegisterSeat(const FString& SeatId , const FString& AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/seats/seatId/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostRegisterSeat);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResPostRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200 )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());


		// JSON 응답 파싱
		FString ResponseBody = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

		if ( FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid() )
		{
			// "response" 객체에 접근
			TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

			if ( ResponseObject.IsValid() )
			{
				// 받아올 정보 추출
				int32 RemainingTicket = ResponseObject->GetNumberField(TEXT("remainingTicket"));

				ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
				if ( TTPlayer )
				{
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if ( GI )
					{
						// 닉네임 설정 및 가져오기
						GI->SetRemainingTicketCount(RemainingTicket);
						UE_LOG(LogTemp , Log , TEXT("RemainingTicket: %d") , GI->GetRemainingTicketCount());
					}
				}

				if ( TicketingUI )
				{
					// 접수 성공했을 때 UI연결
					//TicketingUI->OnSeatRegistered();
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to register seat: %s") , *Response->GetContentAsString());
	}
}

void AHM_HttpActor2::ReqDeleteCancelSeat(const FString& SeatId , const FString& AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/seats/seatId/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResDeleteCancelSeat);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResDeleteCancelSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200 )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( TicketingUI )
		{
			// 접수 취소 성공했을 때 UI연결
			//TicketingUI->OnSeatCanceled(); // 예시 함수
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to cancel seat: %s") , *Response->GetContentAsString());
	}
}

