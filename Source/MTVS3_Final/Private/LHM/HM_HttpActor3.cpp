// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor3.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "ImageUtils.h"
#include "IImageWrapperModule.h"
#include "Async/Async.h"
#include "Engine/Texture2D.h"
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

void AHM_HttpActor3::DownloadStickerImages(const TArray<FString>& StickerImageUrls)
{
	for (const FString& StickerImageUrl : StickerImageUrls)
	{
		DownloadImage(StickerImageUrl, EImageType::StickerImage);
		UE_LOG(LogTemp , Log , TEXT("DownloadStickerImages"));
	}
}

void AHM_HttpActor3::DownloadImage(const FString& URL, EImageType ImageType)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHM_HttpActor3::OnImageDownloaded, ImageType);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
    
	// 진행 중인 이전 요청 취소
	if (ActiveRequests.Contains(ImageType))
	{
		ActiveRequests[ImageType]->CancelRequest();
	}
    
	ActiveRequests.Add(ImageType, HttpRequest);
	HttpRequest->ProcessRequest();
}

void AHM_HttpActor3:: OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, EImageType ImageType)
{
	// 완료된 요청 제거
	ActiveRequests.Remove(ImageType);

	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		TArray<uint8> ImageData = Response->GetContent();
		UTexture2D* Texture = LoadTextureFromData(ImageData);

		if (Texture && TicketCustomUI)
		{
			switch (ImageType)
			{
			case EImageType::StickerImage:
				TicketCustomUI->SetStickerImage(Texture);
				UE_LOG(LogTemp , Log , TEXT("StickerImage->SetImage"));
				break;
			case EImageType::TicketImage:
				//TicketCustomUI->SetStickersImg(Texture);
				UE_LOG(LogTemp , Log , TEXT("TicketImage->SetImage"));
				break;
			case EImageType::BackgroundImage:
				TicketCustomUI->SetBackgroundImg(Texture);
				UE_LOG(LogTemp , Log , TEXT("BackgroundImage->SetImage"));
				break;
			}
		}
	}
}

UTexture2D* AHM_HttpActor3::LoadTextureFromData(const TArray<uint8>& ImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		TArray<uint8> RawData;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
		{
			// 텍스처 생성
			UTexture2D* Texture = UTexture2D::CreateTransient(
				ImageWrapper->GetWidth(), 
				ImageWrapper->GetHeight(), 
				PF_B8G8R8A8
			);

			if (Texture)
			{
				// 텍스처 데이터 설정
				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

				// 텍스처 리소스 업데이트
				Texture->UpdateResource();
				return Texture;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to create texture from image data."));
	return nullptr;
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
						
						// JSON 문자열을 FTitles 구조체로 변환
						FTitles NewTitles;
						if (FJsonObjectConverter::JsonObjectToUStruct(TitlesObject.ToSharedRef() , &NewTitles , 0 , 0))
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

						// JSON 문자열을 FTitles 구조체로 변환
						FStickers NewStikers;
						if (FJsonObjectConverter::JsonObjectToUStruct(StickerObject.ToSharedRef() , &NewStikers , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempStickerItems.Add(NewStikers);
							
							UE_LOG(LogTemp , Log , TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s, StickerImg: %s") ,
								   NewStikers.stickerId ,
								   *NewStikers.stickerName ,
								   *NewStikers.stickerScript ,
								   *NewStikers.stickerRarity ,
								   *NewStikers.stickerImage );
						}
					}
					SetStickerItems(TempStickerItems);
					
					// 티켓 목록
					TArray<TSharedPtr<FJsonValue>> TicketsList = ResponseObject->GetArrayField(TEXT("memberTicketDTOList"));
					TArray<FTickets> TempTicketItems;
					for ( const TSharedPtr<FJsonValue>& TicketValue : TicketsList )
					{
						TSharedPtr<FJsonObject> TicketObject = TicketValue->AsObject();

						// JSON 문자열을 FTitles 구조체로 변환
						FTickets NewTickets;
						if (FJsonObjectConverter::JsonObjectToUStruct(TicketObject.ToSharedRef() , &NewTickets , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempTicketItems.Add(NewTickets);
							
							UE_LOG(LogTemp , Log , TEXT("Tickets Info | Id: %d, Name: %s, Seat: %s, TicketImg: %s") ,
								   NewTickets.ticketId ,
								   *NewTickets.concertName ,
								   *NewTickets.seatInfo ,
								   *NewTickets.ticketImage );
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
				if (ResponseObject.IsValid())
				{
					// 타이틀 정보 처리
					TSharedPtr<FJsonObject> TitleObject = ResponseObject->GetObjectField(TEXT("titleInfo"));
					if (TitleObject.IsValid())
					{
						FTitles NewTitles;
						if (FJsonObjectConverter::JsonObjectToUStruct(TitleObject.ToSharedRef(), &NewTitles, 0, 0))
						{
							UE_LOG(LogTemp, Log, TEXT("Titles Info | Id: %d, Name: %s, Script: %s, Rarity: %s"),
								NewTitles.titleId,
								*NewTitles.titleName,
								*NewTitles.titleScript,
								*NewTitles.titleRarity);

							// 기존 TitleItems 배열에 추가
							TitleItems.Add(NewTitles);
						}
					}

					// 스티커 정보 처리
					TSharedPtr<FJsonObject> StickerObject = ResponseObject->GetObjectField(TEXT("stickerInfo"));
					if (StickerObject.IsValid())
					{
						FStickers NewStickers;
						if (FJsonObjectConverter::JsonObjectToUStruct(StickerObject.ToSharedRef(), &NewStickers, 0, 0))
						{
							UE_LOG(LogTemp, Log, TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s, StickerImg: %s"),
								NewStickers.stickerId,
								*NewStickers.stickerName,
								*NewStickers.stickerScript,
								*NewStickers.stickerRarity,
								*NewStickers.stickerImage);

							// 기존 StickerItems 배열에 추가
							StickerItems.Add(NewStickers);
						}
					}
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
void AHM_HttpActor3::ReqPostSaveCustomTicket(const TArray<uint8>& ImageData, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken)
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

	// ImageData를 Base64로 인코딩
	FString Base64Image = FBase64::Encode(ImageData);

	// JSON 데이터 작성
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("customTicketImage"), Base64Image);  // Base64로 인코딩된 이미지
	Writer->WriteValue(TEXT("backgroundId"), BackGroundId);       // 배경 ID

	// StickerList를 JSON 배열로 작성
	Writer->WriteArrayStart(TEXT("stickerIdList"));
	for (int32 StickerId : StickerList)
	{
		Writer->WriteValue(StickerId);
	}
	Writer->WriteArrayEnd();

	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수 연결
	Request->OnProcessRequestComplete().BindUObject(this, &AHM_HttpActor3::OnResPostBackground);

	// 요청 전송
	Request->ProcessRequest();
}

// 커스텀 티켓 저장 요청에 대한 응답
void AHM_HttpActor3::OnResPostSaveCustomTicket(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					UE_LOG(LogTemp , Log , TEXT("커스텀 티켓 저장 성공 응답"));
				}
			}
		}
	}
}

// 배경 생성 요청
void AHM_HttpActor3::ReqPostBackground(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("Tickets.ticketId: %d"), Tickets.ticketId);
	
	//FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, Tickets.ticketId);
	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/1/background") , *_url); // 임의 티켓아이디
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResPostBackground);

	// 요청 전송
	Request->ProcessRequest();
}

// 배경 생성 요청에 대한 응답
void AHM_HttpActor3::OnResPostBackground(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					FString ImageURL = ResponseObject->GetStringField(TEXT("backgroundImage"));
					SetBackgroundId(_BackgroundId);
					UE_LOG(LogTemp , Log , TEXT("BackgroundId : %d"), GetBackgroundId());
					UE_LOG(LogTemp , Log , TEXT("BackgroundImageURL : %s"), *ImageURL);

					
					// // 이미지 URL 추출
					// FString ImageURL = ResponseObject->GetStringField(TEXT("backgroundImage"));
					// if (!ImageURL.IsEmpty())
					// {
					// 	// URL로 이미지 다운로드
					// 	DownloadImageFromURL(ImageURL);
					// }
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
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetCustomTicketList);

	// 요청 전송
	Request->ProcessRequest();
}

// My 커스텀 티켓 목록 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetCustomTicketList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					// "ticketDTOList" 배열에 접근
					TArray<TSharedPtr<FJsonValue>> TicketList = ResponseObject->GetArrayField(TEXT("ticketDTOList"));

					for (const TSharedPtr<FJsonValue>& TicketValue : TicketList)
					{
						// 각 티켓 정보를 FJsonObject로 변환
						TSharedPtr<FJsonObject> TicketObject = TicketValue->AsObject();
						if (TicketObject.IsValid())
						{
							// 티켓의 기본 정보 추출
							int32 TicketId = TicketObject->GetIntegerField(TEXT("ticketId"));
							FString SeatInfo = TicketObject->GetStringField(TEXT("seatInfo"));
							FString TicketImage = TicketObject->GetStringField(TEXT("ticketImage"));
            
							// concertInfo 필드 접근
							TSharedPtr<FJsonObject> ConcertInfoObject = TicketObject->GetObjectField(TEXT("concertInfo"));
							if (ConcertInfoObject.IsValid())
							{
								FString ConcertName = ConcertInfoObject->GetStringField(TEXT("concertName"));
								int32 Year = ConcertInfoObject->GetIntegerField(TEXT("year"));
								int32 Month = ConcertInfoObject->GetIntegerField(TEXT("month"));
								int32 Day = ConcertInfoObject->GetIntegerField(TEXT("day"));
								FString Time = ConcertInfoObject->GetStringField(TEXT("time"));

								// 각 필드 로그 출력
								UE_LOG(LogTemp, Log, TEXT("Ticket Info | Id: %d, SeatInfo: %s, TicketImage: %s"), TicketId, *SeatInfo, *TicketImage);
								UE_LOG(LogTemp, Log, TEXT("Concert Info | Name: %s, Date: %d-%d-%d, Time: %s"), *ConcertName, Year, Month, Day, *Time);
							}
						}
					}
				}
			}
		}
	}
}

// 티켓 커스텀 제작 입장 요청
void AHM_HttpActor3::ReqGetEnterTicketCustomization(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("Tickets.ticketId: %d"), Tickets.ticketId);
	
	//FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/custom") , *_url, Tickets.ticketId);
	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/1/custom") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetEnterTicketCustomization);

	// 요청 전송
	Request->ProcessRequest();
}

// 티켓 커스텀 제작 입장 요청에 대한 응답
void AHM_HttpActor3::OnResGetEnterTicketCustomization(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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

				if (ResponseObject.IsValid())
				{
					int32 DailyBackgroundRefreshCount = ResponseObject->GetIntegerField(TEXT("dailyBackgroundRefreshCount"));
					UE_LOG(LogTemp , Log , TEXT("DailyBackgroundRefreshCount: %d") , DailyBackgroundRefreshCount);

					// "stickerDTOList" 배열 필드에 접근
					TArray<TSharedPtr<FJsonValue>> StickerList = ResponseObject->GetArrayField(TEXT("stickerDTOList"));
					TArray<FString> StickerImageUrls;
					for (const TSharedPtr<FJsonValue>& StickerValue : StickerList)
					{
						// 각 스티커 정보를 FJsonObject로 변환
						TSharedPtr<FJsonObject> StickerObject = StickerValue->AsObject();
						if (StickerObject.IsValid())
						{
							int32 StickerId = StickerObject->GetIntegerField(TEXT("stickerId"));
							FString StickerImage = StickerObject->GetStringField(TEXT("stickerImage"));
							UE_LOG(LogTemp , Log , TEXT("StickerId: %d, StickerImage: %s"), StickerId, *StickerImage);
							StickerImageUrls.Add(StickerImage);
						}
					}
					// 스티커 이미지 다운로드 요청
					DownloadStickerImages(StickerImageUrls);
				}
			}
		}
	}
}
