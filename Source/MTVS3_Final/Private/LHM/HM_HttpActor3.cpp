// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor3.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "JMH/MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"

// Sets default values
AHM_HttpActor3::AHM_HttpActor3()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHM_HttpActor3::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHM_HttpActor3::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 전달받은 MainUI 참조 저장
void AHM_HttpActor3::SetMainUI(UMainWidget* InMainUI)
{
	MainUI = InMainUI;
}

void AHM_HttpActor3::ReqGetInventoryData(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/inventory") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetInventoryData);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor3::OnResGetInventoryData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
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
					// 칭호 목록
					TArray<TSharedPtr<FJsonValue>> TitlesList = ResponseObject->GetArrayField(TEXT("memberTitleDTOList"));
					TArray<FTitles> TempTitleItems;
					for ( const TSharedPtr<FJsonValue>& TitlesValue : TitlesList )
					{
						TSharedPtr<FJsonObject> TitlesObject = TitlesValue->AsObject();
						
						// TitlesObject를 JSON 문자열로 변환
						FString TitlesJsonString;
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&TitlesJsonString);
						FJsonSerializer::Serialize(TitlesObject.ToSharedRef() , Writer);

						// JSON 문자열을 FTitles 구조체로 변환
						FTitles NewTitles;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(TitlesJsonString , &NewTitles , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempTitleItems.Add(NewTitles);
							
							UE_LOG(LogTemp , Log , TEXT("Titles Info | Id: %d, Name: %s, Script: %s, Rarity: %s, Representative: %s") ,
							       NewTitles.titleId ,
							       *NewTitles.titleName ,
							       *NewTitles.titleScript ,
							       *NewTitles.titleRarity,
							       NewTitles.bIsEquipped ? TEXT("true") : TEXT("false"));
						}
					}
					SetTitleItems(TempTitleItems);

					// 스티커 목록
					TArray<TSharedPtr<FJsonValue>> StickersList = ResponseObject->GetArrayField(TEXT("memberStickerDTOList"));
					TArray<FStickers> TempStickerItems;
					for ( const TSharedPtr<FJsonValue>& StickerValue : StickersList )
					{
						TSharedPtr<FJsonObject> StickerObject = StickerValue->AsObject();
						
						// TitlesObject를 JSON 문자열로 변환
						FString StickerJsonString;
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&StickerJsonString);
						FJsonSerializer::Serialize(StickerObject.ToSharedRef() , Writer);

						// JSON 문자열을 FTitles 구조체로 변환
						FStickers NewStikers;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(StickerJsonString , &NewStikers , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempStickerItems.Add(NewStikers);
							
							UE_LOG(LogTemp , Log , TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s") ,
								   NewStikers.stickerId ,
								   *NewStikers.stickerName ,
								   *NewStikers.stickerScript ,
								   *NewStikers.stickerRarity );
						}
					}
					SetStickerItems(TempStickerItems);
					
					// 스티커 목록
					TArray<TSharedPtr<FJsonValue>> TicketsList = ResponseObject->GetArrayField(TEXT("memberTicketDTOList"));
					TArray<FTickets> TempTicketItems;
					for ( const TSharedPtr<FJsonValue>& TicketValue : TicketsList )
					{
						TSharedPtr<FJsonObject> TicketObject = TicketValue->AsObject();
						
						// TitlesObject를 JSON 문자열로 변환
						FString TicketJsonString;
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&TicketJsonString);
						FJsonSerializer::Serialize(TicketObject.ToSharedRef() , Writer);

						// JSON 문자열을 FTitles 구조체로 변환
						FTickets NewTickets;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(TicketJsonString , &NewTickets , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempTicketItems.Add(NewTickets);
							
							UE_LOG(LogTemp , Log , TEXT("Tickets Info | Id: %d, Name: %s, Seat: %s") ,
								   NewTickets.tickerId ,
								   *NewTickets.concertName ,
								   *NewTickets.seatInfo );
						}
					}
					SetTicketItems(TempTicketItems);

					if (MainUI)
					{
						//MainUI
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to get concert entry: %s") , *Response->GetContentAsString());
		}
	}
}

void AHM_HttpActor3::ReqPostPuzzleResultAndGetSticker(int32 Rank, FString AccessToken)
{
	
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/puzzle/result") , *_url, HttpActor2->GetConcertId());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("rank") , Rank);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);
	
	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResPostPuzzleResultAndGetSticker);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor3::OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
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
					// 스티커 목록
					TArray<TSharedPtr<FJsonValue>> StickersList = ResponseObject->GetArrayField(TEXT("memberStickerDTOList"));
					TArray<FStickers> NewStickerItems;
					for ( const TSharedPtr<FJsonValue>& StickerValue : StickersList )
					{
						TSharedPtr<FJsonObject> StickerObject = StickerValue->AsObject();
						
						// TitlesObject를 JSON 문자열로 변환
						FString StickerJsonString;
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&StickerJsonString);
						FJsonSerializer::Serialize(StickerObject.ToSharedRef() , Writer);

						// JSON 문자열을 FTitles 구조체로 변환
						FStickers NewStikers;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(StickerJsonString , &NewStikers , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							NewStickerItems.Add(NewStikers);
							
							UE_LOG(LogTemp , Log , TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s") ,
								   NewStikers.stickerId ,
								   *NewStikers.stickerName ,
								   *NewStikers.stickerScript ,
								   *NewStikers.stickerRarity );
						}
					}
					// 기존 StickerItems 배열에 새 항목 추가
					StickerItems.Append(NewStickerItems);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Puzzle Result And Get Sticker: %s") , *Response->GetContentAsString());
		}
	}
}

void AHM_HttpActor3::ReqPostSaveCustomTicket(UTexture2D* CustomTicket, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, Tickets.tickerId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	//Writer->WriteValue(TEXT("customTicketImage") , CustomTicket);
	//Writer->WriteValue(TEXT("stickerIdList") , StickerList);
	//Writer->WriteValue(TEXT("backgroundId") , BackGroundId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);
	
	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnReqGetBackground);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor3::ReqGetBackground(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, Tickets.tickerId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnReqGetBackground);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor3::OnReqGetBackground(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}