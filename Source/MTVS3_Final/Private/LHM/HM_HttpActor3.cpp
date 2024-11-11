// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor3.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "JMH/MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"
#include "LHM/HM_TicketCustom.h"

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
	
	if (TicketCustomWidget)
	{
		TicketCustomUI = CreateWidget<UHM_TicketCustom>(GetWorld(), TicketCustomWidget);
	}
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

// 인벤토리 정보 요청
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

// 인벤토리 정보 요청에 대한 응답
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
								   NewTickets.ticketId ,
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

// Puzzle 결과, Sticker 획득 요청
void AHM_HttpActor3::ReqPostPuzzleResultAndGetSticker(int32 Rank, FString AccessToken)
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));

	if(HttpActor2)
	{
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
}

// Puzzle 결과, Sticker 획득 요청에 대한 응답
void AHM_HttpActor3::OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

// 커스텀 티켓 저장 요청
void AHM_HttpActor3::ReqPostSaveCustomTicket(UTexture2D* CustomTicket, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, Tickets.ticketId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

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

// 커스텀 티켓 저장 요청에 대한 응답
void AHM_HttpActor3::OnReqPostSaveCustomTicket(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					// 티켓 저장 성공처리
				}
			}
		}
	}
}

// 배경 생성 요청
void AHM_HttpActor3::ReqGetBackground(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, Tickets.ticketId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnReqGetBackground);

	// 요청 전송
	Request->ProcessRequest();
}

// 배경 생성 요청에 대한 응답
void AHM_HttpActor3::OnReqGetBackground(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					// 받아올 정보 추출
					int32 _BackgroundId = ResponseObject->GetIntegerField(TEXT("backgroundId"));
					SetBackgroundId(_BackgroundId);
					UE_LOG(LogTemp , Log , TEXT("backgroundId : %d"), GetBackgroundId());

					// Base64로 인코딩된 이미지 데이터 추출
					FString Base64Image = ResponseObject->GetStringField(TEXT("backgroundImage"));
					TArray<uint8> ImageData;
					FBase64::Decode(Base64Image , ImageData);

					// 텍스처로 변환
					UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
					if ( Texture )
					{
						if ( TicketCustomUI )
						{
							TicketCustomUI->SetBackgroundImg(Texture);
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
					}
				}
			}
		}
	}
}

// My 커스텀 티켓 목록 조회 요청
void AHM_HttpActor3::ReqGetCustomTicketList(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnReqGetCustomTicketList);

	// 요청 전송
	Request->ProcessRequest();
}

// My 커스텀 티켓 목록 조회 요청에 대한 응답
void AHM_HttpActor3::OnReqGetCustomTicketList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					TSharedPtr<FJsonObject> CustomTicketListObject = ResponseObject->GetObjectField(TEXT("ticketDTOList"));
					if ( CustomTicketListObject.IsValid() )
					{
						int32 TicketId = CustomTicketListObject->GetIntegerField(TEXT("ticketId"));
						FString SeatInfo = CustomTicketListObject->GetStringField(TEXT("seatInfo"));
						UE_LOG(LogTemp , Log , TEXT("TicketId: %d, SeatInfo: %s") , TicketId , *SeatInfo);
					}
					
					TSharedPtr<FJsonObject> ConcertInfoObject = ResponseObject->GetObjectField(TEXT("concertInfo"));
					if ( ConcertInfoObject.IsValid() )
					{
						FString ConcertName = CustomTicketListObject->GetStringField(TEXT("concertName"));
						int32 Year = CustomTicketListObject->GetIntegerField(TEXT("year"));
						int32 Month = CustomTicketListObject->GetIntegerField(TEXT("month"));
						int32 Day = CustomTicketListObject->GetIntegerField(TEXT("day"));
						FString Time = CustomTicketListObject->GetStringField(TEXT("time"));
						UE_LOG(LogTemp , Log , TEXT("ConcertName: %s, Date: %d-%d-%d-%s") , *ConcertName, Year, Month, Day, *Time);
					}
				}
			}
		}
	}
}

// 티켓 커스텀 제작 입장 요청
void AHM_HttpActor3::ReqPostEnterTicketCustomization(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/custom") , *_url, Tickets.ticketId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnReqPostEnterTicketCustomization);

	// 요청 전송
	Request->ProcessRequest();
}

// 티켓 커스텀 제작 입장 요청에 대한 응답
void AHM_HttpActor3::OnReqPostEnterTicketCustomization(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
						int32 DailyBackgroundRefreshCount = ResponseObject->GetIntegerField(TEXT("dailyBackgroundRefreshCount"));
						UE_LOG(LogTemp , Log , TEXT("DailyBackgroundRefreshCount: %d") , DailyBackgroundRefreshCount);
					
					TSharedPtr<FJsonObject> StickerListObject = ResponseObject->GetObjectField(TEXT("stickerDTOList"));
					if ( StickerListObject.IsValid() )
					{
						int32 StickerId = StickerListObject->GetIntegerField(TEXT("stickerId"));
						UE_LOG(LogTemp , Log , TEXT("StickerId: %d") , StickerId);

						// Base64로 인코딩된 이미지 데이터 추출
						FString Base64Image = StickerListObject->GetStringField(TEXT("stickerImage"));
						TArray<uint8> ImageData;
						FBase64::Decode(Base64Image , ImageData);

						// 텍스처로 변환
						UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
						if ( Texture )
						{
							if ( TicketCustomUI )
							{
								//TicketCustomUI->SetStickersImg(Texture1, Texture2, Texture3, Texture4, Texture5);
								UE_LOG(LogTemp , Warning , TEXT("Success to create texture from image data."));
							}
						}
						else
						{
							UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
						}
					}
				}
			}
		}
	}
}
