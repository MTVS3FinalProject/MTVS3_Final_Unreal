﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor3.h"

#include "EngineUtils.h"
#include "HttpModule.h"
#include "ImageUtils.h"
#include "Async/Async.h"
#include "Engine/Texture2D.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "Components/CanvasPanel.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTHallGameState.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_Inventory.h"
#include "JMH/MH_NoticeWidget.h"
#include "JMH/MH_TicketingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"
#include "LHM/HM_PuzzleWidget.h"
#include "LHM/HM_TicketCustom.h"
#include "LHM/HM_Tree.h"
#include "LHM/HM_TreeCustomTicketWidget.h"
#include "LHM/PuzzleManager.h"
#include "LHM/TTPlayerController.h"

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

void AHM_HttpActor3::SetTicketingUI(UMH_TicketingWidget* InTicketingUI)
{
	TicketingUI = InTicketingUI;
}

// void AHM_HttpActor3::SetPuzzleUI(UHM_PuzzleWidget* InPuzzleUI)
// {
// 	PuzzleUI = InPuzzleUI;
// }

void AHM_HttpActor3::SetTreeTicketUI(UHM_TreeCustomTicketWidget* InTreeTicketUI)
{
	TreeTicketUI = InTreeTicketUI;
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
							
							UE_LOG(LogTemp , Log , TEXT("Tickets Info | Id: %d, Name: %s, Seat: %s, TicketImg: %s, Date: %d-%d-%d-%s") ,
								   NewTickets.ticketId ,
								   *NewTickets.concertName ,
								   *NewTickets.seatInfo ,
								   *NewTickets.ticketImage ,
								   NewTickets.year ,
								   NewTickets.month ,
								   NewTickets.day ,
								   *NewTickets.time );
						}
						SetTicketId(NewTickets.ticketId);
					}
					
					SetTicketItems(TempTicketItems);
					if (MainUI)
					{
						MainUI->WBP_MH_MainBar->WBP_inventoryUI->InitializeTabs();
						MainUI->WBP_MH_MainBar->SetVisibilityState();
						UE_LOG(LogTemp , Log , TEXT("inventoryUI->InitializeTabs()"));
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
TMap<TSharedPtr<IHttpRequest>, int32> RequestRankMap;
TMap<TSharedPtr<IHttpRequest>, FString> RequestPlayerNicknameMap;
void AHM_HttpActor3::ReqPostPuzzleResultAndGetSticker(int32 Rank, FString Nickname, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("Puzzle 결과, Sticker 획득 요청"));
	
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

		int32 NewRank = Rank + 1;
		// 전달 데이터 (JSON)
		FString ContentString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
		Writer->WriteObjectStart();
		Writer->WriteValue(TEXT("rank") , NewRank);
		Writer->WriteObjectEnd();
		Writer->Close();

		// 요청 본문에 JSON 데이터를 설정
		Request->SetContentAsString(ContentString);

		// Rank를 추적
		RequestRankMap.Add(Request, Rank);
		RequestPlayerNicknameMap.Add(Request, Nickname);
		
		// 응답받을 함수를 연결
		Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResPostPuzzleResultAndGetSticker);

		// 요청 전송
		Request->ProcessRequest();
	}
}

// Puzzle 결과, Sticker 획득 요청에 대한 응답
void AHM_HttpActor3::OnResPostPuzzleResultAndGetSticker(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	int32 Rank = RequestRankMap.Contains(Request) ? RequestRankMap[Request] : -1;
	FString PlayerNickname = RequestPlayerNicknameMap.Contains(Request) ? RequestPlayerNicknameMap[Request] : TEXT("");
	// Rank 맵에서 제거
	RequestRankMap.Remove(Request);
	RequestPlayerNicknameMap.Remove(Request);
	
	if (Rank < 0 || Rank > 2)
	{
		UE_LOG(LogTemp , Warning , TEXT("Invalid rank"));
		return;
	}

	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if (Response->GetResponseCode() == 200)
		{
			// JSON 응답 파싱
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			if (FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					FStickers NewStickers;
					FTitles NewTitles;

					// 타이틀 정보 처리
					TSharedPtr<FJsonObject> TitleObject = ResponseObject->GetObjectField(TEXT("titleInfo"));
					if (TitleObject.IsValid())
					{
						if (FJsonObjectConverter::JsonObjectToUStruct(TitleObject.ToSharedRef() , &NewTitles))
						{
							UE_LOG(LogTemp , Log , TEXT("Titles Info | Id: %d, Name: %s, Script: %s, Rarity: %s") ,
							       NewTitles.titleId ,
							       *NewTitles.titleName ,
							       *NewTitles.titleScript ,
							       *NewTitles.titleRarity);

							// 기존 TitleItems 배열에 추가
							TitleItems.Add(NewTitles);
						}
					}

					// 스티커 정보 처리
					TSharedPtr<FJsonObject> StickerObject = ResponseObject->GetObjectField(TEXT("stickerInfo"));
					if (StickerObject.IsValid())
					{
						if (FJsonObjectConverter::JsonObjectToUStruct(StickerObject.ToSharedRef() , &NewStickers))
						{
							UE_LOG(LogTemp , Log ,
							       TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s, StickerImg: %s") ,
							       NewStickers.stickerId ,
							       *NewStickers.stickerName ,
							       *NewStickers.stickerScript ,
							       *NewStickers.stickerRarity ,
							       *NewStickers.stickerImage);

							// 기존 StickerItems 배열에 추가
							StickerItems.Add(NewStickers);
						}
					}

					// stickerImage URL 가져오기
					FString StickerImageUrl = StickerObject->GetStringField(TEXT("stickerImage"));

					// 상위 3명 데이터를 수집
					TArray<FPlayerRankInfo> RankInfos;
					for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
					{
						ATTPlayer* TTPlayer = *It;
						if (TTPlayer && TTPlayer->GetNickname() == PlayerNickname)
						{
							FPlayerRankInfo RankInfo;
							RankInfo.NickName = PlayerNickname;
							RankInfo.Rank = Rank;
							RankInfo.StickerImageUrl = StickerImageUrl; // URL 저장
							RankInfo.StickerRarity = NewStickers.stickerRarity;
							RankInfo.StickerName = NewStickers.stickerName;
							RankInfo.StickerScript = NewStickers.stickerScript;
							RankInfo.TitleRarity = NewTitles.titleRarity;
							RankInfo.TitleName = NewTitles.titleName;
							RankInfo.TitleScript = NewTitles.titleScript;

							RankInfos.Add(RankInfo);
						}
					}
					
					// // 스티커, 타이틀 정보 퍼즐 UI Update
					// for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
					// {
					// 	ATTPlayer* TTPlayer = *It;
					// 	if (TTPlayer)
					// 	{
					// 		TTPlayer->Multicast_UpdateAllPuzzleRanks(RankInfos);
					// 	}
					// }
					
					// 우편함 알림표시 Update
					APuzzleManager* PuzzleManager = Cast<APuzzleManager>(
						UGameplayStatics::GetActorOfClass(GetWorld() , APuzzleManager::StaticClass()));
					if (PuzzleManager)
					{
						PuzzleManager->Server_HandlePuzzleResult();
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Puzzle Result And Get Sticker: %s") ,
			       *Response->GetContentAsString());
		}
	}
}

// 커스텀 티켓 저장 요청 - Multipartfile
void AHM_HttpActor3::ReqPostSaveCustomTicketMultipart(const TArray<uint8>& ImageData, TArray<int32> StickerList, int32 BackGroundId, FString AccessToken)
{
	UE_LOG(LogTemp, Log, TEXT("커스텀 티켓 저장 요청 - ImageData Multipart"));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	
    // HTTP 모듈 가져오기
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http) return;

    // HTTP 요청 생성
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/custom"), *_url, GetTicketId());
    Request->SetURL(FormattedUrl);
    Request->SetVerb(TEXT("POST"));

    // 헤더 설정
    FString Boundary = "-------------BoundaryString"; // 임의의 고유 문자열
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));
	Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));

	// 멀티파트 폼 데이터 작성
	TArray<uint8> RequestContent;

	// 1. Boundary 시작
	FString BoundaryStart = FString::Printf(TEXT("--%s\r\n"), *Boundary);
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*BoundaryStart), BoundaryStart.Len());

	// 2. 이미지 데이터 추가
	FString HeaderImage = TEXT("Content-Disposition: form-data; name=\"customTicketImage\"; filename=\"ticket.png\"\r\nContent-Type: image/png\r\n\r\n");
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*HeaderImage), HeaderImage.Len());
	RequestContent.Append(ImageData); // 이미지 데이터를 그대로 추가
	RequestContent.Append((uint8*)"\r\n", 2);
	
	// 3. Start_x 추가
	FString Start_x = FString::Printf(TEXT("--%s\r\n"), *Boundary);
	Start_x += TEXT("Content-Disposition: form-data; name=\"start_x\"\r\n\r\n");
	Start_x += FString::FromInt(856) + TEXT("\r\n");
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*Start_x), Start_x.Len());
	
	// 4. Start_y 추가
	FString Start_y = FString::Printf(TEXT("--%s\r\n"), *Boundary);
	Start_y += TEXT("Content-Disposition: form-data; name=\"start_y\"\r\n\r\n");
	Start_y += FString::FromInt(96) + TEXT("\r\n");
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*Start_y), Start_y.Len());
	
	// 5. BackgroundId 추가
	FString BackgroundIdPart = FString::Printf(TEXT("--%s\r\n"), *Boundary);
	BackgroundIdPart += TEXT("Content-Disposition: form-data; name=\"backgroundId\"\r\n\r\n");
	BackgroundIdPart += FString::FromInt(BackGroundId) + TEXT("\r\n");
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*BackgroundIdPart), BackgroundIdPart.Len());

	// 6. StickerIdList 추가
	FString StickerListPart = FString::Printf(TEXT("--%s\r\n"), *Boundary);
	StickerListPart += TEXT("Content-Disposition: form-data; name=\"stickerIdList\"\r\n\r\n");
	for (int32 StickerId : StickerList)
	{
		StickerListPart += FString::FromInt(StickerId) + TEXT(",");
	}
	StickerListPart.RemoveFromEnd(TEXT(","));
	StickerListPart += TEXT("\r\n");
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*StickerListPart), StickerListPart.Len());

	// 7. Boundary 종료
	FString BoundaryEnd = FString::Printf(TEXT("--%s--\r\n"), *Boundary);
	RequestContent.Append((uint8*)TCHAR_TO_ANSI(*BoundaryEnd), BoundaryEnd.Len());

	// 요청 본문에 데이터 추가
	Request->SetContent(RequestContent);
	
    // 응답받을 함수 연결
    Request->OnProcessRequestComplete().BindUObject(this, &AHM_HttpActor3::OnResPostSaveCustomTicket);

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
				UE_LOG(LogTemp , Log , TEXT("커스텀 티켓 저장 성공 응답"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("커스텀 티켓 저장 실패"));
		}
	}
}

// 배경 생성 요청
void AHM_HttpActor3::ReqPostBackground(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("배경 생성 요청"));
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	UE_LOG(LogTemp , Log , TEXT("AccessToken: %s"), *AccessToken);
	//FTickets Tickets;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("TicketId: %d"), GetTicketId());
	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/background") , *_url, GetTicketId());
	//FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/1/background") , *_url);

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

			if (FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid())
			{
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					// 받아올 정보 추출
					int32 _BackgroundId = ResponseObject->GetIntegerField(TEXT("backgroundId"));
					UE_LOG(LogTemp , Log , TEXT("BackgroundId : %d") , _BackgroundId);

					SetBackgroundId(_BackgroundId);

					// stickerImage URL 가져오기
					FString BackgroundImageUrl = ResponseObject->GetStringField(TEXT("backgroundImage"));

					// URL에서 이미지 다운로드
					TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
					ImageRequest->SetURL(BackgroundImageUrl);
					ImageRequest->SetVerb(TEXT("GET"));

					// 다운로드 완료 시 콜백 설정
					ImageRequest->OnProcessRequestComplete().BindLambda(
						[this](FHttpRequestPtr ImageRequest , FHttpResponsePtr ImageResponse , bool bImageSuccessful)
						{
							if (bImageSuccessful && ImageResponse.IsValid())
							{
								// 이미지 데이터 가져오기
								TArray<uint8> ImageData = ImageResponse->GetContent();

								// 데이터로부터 텍스처 생성
								UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
								if (Texture)
								{
									if (MainUI->GetTicketCustomWidget())
									{
										MainUI->GetTicketCustomWidget()->SetBackgroundImg(Texture);
										UE_LOG(LogTemp , Log , TEXT("배경 생성 요청 응답 성공"));
									}
								}
							}
							else
							{
								UE_LOG(LogTemp , Warning , TEXT("Failed to download Background image"));
							}
						});
					ImageRequest->ProcessRequest();
				}
			}
		}
	}
}

// 티켓 커스텀 제작 입장 요청
void AHM_HttpActor3::ReqGetEnterTicketCustomization(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("티켓 커스텀 제작 입장 요청"));
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("TicketId: %d"), GetTicketId());
	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/%d/custom") , *_url, GetTicketId());
	//FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/1/custom") , *_url); // 테스트용
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

					TArray<TSharedPtr<FJsonValue>> TicketDtoList = ResponseObject->GetArrayField(TEXT("ticketDTOList"));
					for (int32 i = 0; i < TicketDtoList.Num(); i++)
					{
						TSharedPtr<FJsonObject> TicketObject = TicketDtoList[i]->AsObject();
						if (TicketObject.IsValid())
						{
							int32 TicketId = TicketObject->GetIntegerField(TEXT("ticketId"));
							FString ConcertName = TicketObject->GetStringField(TEXT("concertName"));
							int32 Year = TicketObject->GetIntegerField(TEXT("year"));
							int32 Month = TicketObject->GetIntegerField(TEXT("month"));
							int32 Day = TicketObject->GetIntegerField(TEXT("day"));
							FString Time = TicketObject->GetStringField(TEXT("time"));
							FString SeatInfo = TicketObject->GetStringField(TEXT("seatInfo"));

							if(MainUI->GetTicketCustomWidget())
							{
								MainUI->GetTicketCustomWidget()->SetTextTicketInfo(ConcertName, Year, Month, Day, Time, SeatInfo);
							}
						}
					}
					
					TArray<TSharedPtr<FJsonValue>> StickerList = ResponseObject->GetArrayField(TEXT("stickerDTOList"));
					
					if(MainUI->GetTicketCustomWidget())
					{
						// 스티커 개수에 따라 UI만 초기화 (한 번만 호출)
						MainUI->GetTicketCustomWidget()->InitializeStickerImages(StickerList.Num());
					}

					for (int32 i = 0; i < StickerList.Num(); i++)
					{
						TSharedPtr<FJsonObject> StickerObject = StickerList[i]->AsObject();
						if (StickerObject.IsValid())
						{
							// stickerId 가져오기
							int32 StickerId = StickerObject->GetIntegerField(TEXT("stickerId"));
							// stickerImage URL 가져오기
							FString StickerImageUrl = StickerObject->GetStringField(TEXT("stickerImage"));

							// URL에서 이미지 다운로드
							TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
							ImageRequest->SetURL(StickerImageUrl);
							ImageRequest->SetVerb(TEXT("GET"));

							// 다운로드 완료 시 콜백 설정
							ImageRequest->OnProcessRequestComplete().BindLambda([this, i](FHttpRequestPtr ImageRequest, FHttpResponsePtr ImageResponse, bool bImageSuccessful) {
								if (bImageSuccessful && ImageResponse.IsValid())
								{
									// 이미지 데이터 가져오기
									TArray<uint8> ImageData = ImageResponse->GetContent();

									// 데이터로부터 텍스처 생성
									UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
									if (Texture)
									{
										// UI 업데이트
										if (MainUI->GetTicketCustomWidget())
										{
											MainUI->GetTicketCustomWidget()->SetStickersImgs(Texture, i);
											//MainUI->GetTicketCustomWidget()->AddSticker(StickerId, Texture);
										}
									}
									else
									{
										UE_LOG(LogTemp, Warning, TEXT("Failed to create texture for sticker index: %d"), i);
									}
								}
								else
								{
									UE_LOG(LogTemp, Warning, TEXT("Failed to download sticker image for index: %d"), i);
								}
							});

							ImageRequest->ProcessRequest();
						}
					}
						if(MainUI->GetTicketCustomWidget()) MainUI->SetWidgetSwitcher(7);
				}
			}
		}
		else if( Response->GetResponseCode() == 400 )
		{
			// 티켓을 보유하고 있지 않을 때 커스텀 티켓 제작에 입장할 수 없는 문구 위젯스위쳐
			MainUI->ShowMainErrorMessage(TEXT("커스텀 할 수 있는 티켓을 소유하고 있지 않습니다."));
		}
	}
}

// 타이틀 장착 요청
void AHM_HttpActor3::ReqGetEquipTheTitle(int32 TitleID, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("타이틀 장착 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;
	
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	
	FString FormattedUrl = FString::Printf(TEXT("%s/member/title/%d") , *_url, TitleID);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("PUT"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetEquipTheTitle);

	// 요청 전송
	Request->ProcessRequest();
}

// 타이틀 장착 요청에 대한 응답
void AHM_HttpActor3::OnResGetEquipTheTitle(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					FString TitleName = ResponseObject->GetStringField(TEXT("titleName"));
					FString TitleRarity = ResponseObject->GetStringField(TEXT("titleRarity"));

					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					ATTPlayer* Player = Cast<ATTPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
					if (Player && GI)
					{
						Player->SetTitleNameAndRarity(TitleName, TitleRarity);
						GI->SetTitleName(TitleName);
						GI->SetTitleRarity(TitleRarity);
					}
					UE_LOG(LogTemp , Log , TEXT("타이틀 장착 성공"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("타이틀 장착 실패"));
		}
	}
}

// 타이틀 해제 요청
void AHM_HttpActor3::ReqGetNotEquipTheTitle(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("타이틀 해제 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/title") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetNotEquipTheTitle);

	// 요청 전송
	Request->ProcessRequest();
}

// 타이틀 해체 요청에 대한 응답
void AHM_HttpActor3::OnResGetNotEquipTheTitle(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
			ATTPlayer* Player = Cast<ATTPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
			if (Player)
			{
				FString TitleName = " ";
				FString TitleRarity = " ";
				Player->SetTitleNameAndRarity(TitleName , TitleRarity);
			}
			UE_LOG(LogTemp , Log , TEXT("타이틀 해제 성공"));
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("타이틀 해제 실패"));
		}
	}
}

// 좌석 결제 미루기 요청
void AHM_HttpActor3::ReqPostponePaymentSeat(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("좌석 결제 미루기 요청"));
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s/postpone") , *_url, HttpActor2->GetConcertId(), *GI->GetLuckyDrawSeatID());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResPostponePaymentSeat);
	
	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor3::OnResPostponePaymentSeat(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
			// GameInstance에서 SeatID 가져오기
			UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
			if (!GI) return;

			FString SeatId = GI->GetLuckyDrawSeatID();
            
			// 서버에 RPC로 요청
			if (ATTPlayerController* PC = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController()))
			{
				PC->ServerUpdateNewSeats(SeatId);
			}

			// UI 업데이트
			if (TicketingUI)
			{
				TArray<int32> UIReservedIndices;
				UIReservedIndices.Add(FCString::Atoi(*SeatId) - 1);  // UI는 0-based 인덱스 사용
				TicketingUI->AddReservedSeatsUI(UIReservedIndices);
			}

			// 우편함 알림 표시
			if (MainUI && MainUI->WBP_MH_MainBar && MainUI->WBP_MH_MainBar->Image_Notice)
			{
				UE_LOG(LogTemp, Log, TEXT("우편함 알림표시 SetVisivle"));
				MainUI->WBP_MH_MainBar->Image_Notice->SetVisibility(ESlateVisibility::Visible);
			}
			UE_LOG(LogTemp, Log, TEXT("좌석 결제 미루기 성공"));
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("좌석 결제 미루기 실패"));
		}
	}
}

// 우편함 조회 요청
void AHM_HttpActor3::ReqGetMailbox(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("우편함 조회 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/mails") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetPostponePaymentSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 우편함 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetPostponePaymentSeat(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					// 메일 목록
					TArray<TSharedPtr<FJsonValue>> MailList = ResponseObject->GetArrayField(TEXT("mailListDTO"));
					TArray<FMails> TempMails;
					for ( const TSharedPtr<FJsonValue>& MailValue : MailList )
					{
						TSharedPtr<FJsonObject> MailObject = MailValue->AsObject();

						// JSON 문자열을 FTitles 구조체로 변환
						FMails NewMails;
						if (FJsonObjectConverter::JsonObjectToUStruct(MailObject.ToSharedRef() , &NewMails , 0 , 0))
						{
							// 변환된 FTitles 구조체를 임시 배열에 추가
							TempMails.Add(NewMails);
							
							UE_LOG(LogTemp , Log , TEXT("Mail Info | Id: %d, Subject: %s, Category: %s, IsRead: %s") ,
								   NewMails.mailId ,
								   *NewMails.subject ,
								   *NewMails.mailCategory ,
								   NewMails.isRead ? TEXT("true") : TEXT("false"));
						}
					}
					SetMails(TempMails);
					if (MainUI)
					{
						MainUI->WBP_MH_MainBar->WBP_NoticeUI->InitializeMessageTabs();
						MainUI->WBP_MH_MainBar->Image_Notice->SetVisibility(ESlateVisibility::Hidden);
					}
				}

				UE_LOG(LogTemp , Log , TEXT("우편함 조회 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("우편함 조회 실패"));
		}
	}
}

// 특정 우편함 조회 요청
void AHM_HttpActor3::ReqGetSpecificMail(int32 MailId, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("특정 우편함 조회 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/mails/%d") , *_url, MailId); 
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetSpecificMail);

	// 요청 전송
	Request->ProcessRequest();
}

// 특정 우편함 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetSpecificMail(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					int32 MailId = ResponseObject->GetIntegerField(TEXT("mailId"));
					FString Subject = ResponseObject->GetStringField(TEXT("subject"));
					FString Content = ResponseObject->GetStringField(TEXT("content"));
					FString MailCategory = ResponseObject->GetStringField(TEXT("mailCategory"));

					UE_LOG(LogTemp , Log , TEXT("MailId: %d") , MailId);
					UE_LOG(LogTemp , Log , TEXT("Subject: %s") , *Subject)
					UE_LOG(LogTemp , Log , TEXT("Content: %s") , *Content);
					UE_LOG(LogTemp , Log , TEXT("MailCategory: %s") , *MailCategory);

					MainUI->WBP_MH_MainBar->WBP_NoticeUI->OnMailDetailReceived(false, Subject, Content);
				}
				UE_LOG(LogTemp , Log , TEXT("특정 우편함 조회 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("특정 우편함 조회 실패"));
		}
	}
}

// 좌석 결제 미루기 우편 조회 요청
void AHM_HttpActor3::ReqGetPostponePaymentSeatMail(int32 MailId, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("좌석 결제 미루기 우편 조회 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/mails/%d/postpone") , *_url, MailId); 
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetPostponePaymentSeatMail);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 결제 미루기 우편 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetPostponePaymentSeatMail(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					int32 ConcertId = ResponseObject->GetIntegerField(TEXT("concertId"));
					int32 SeatId = ResponseObject->GetIntegerField(TEXT("seatId"));

					UE_LOG(LogTemp , Log , TEXT("concertId: %d") , ConcertId);
					UE_LOG(LogTemp , Log , TEXT("seatId: %d") , SeatId)

					AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
							UGameplayStatics::GetActorOfClass(GetWorld(), AHM_HttpActor2::StaticClass()));
					if(HttpActor2)
					{
						HttpActor2->SetPostponeSeatId(SeatId);
						FString PostponeSeatId = FString::FromInt(SeatId);
						GI->SetLuckyDrawSeatID(PostponeSeatId);
					}
				}
				UE_LOG(LogTemp , Log , TEXT("좌석 결제 미루기 우편 조회 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("좌석 결제 미루기 우편 조회 실패"));
		}
	}
}

// 퍼즐 우편 조회 요청
void AHM_HttpActor3::ReqGetPuzzleMail(int32 MailId, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("퍼즐 우편 조회 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/mails/%d/puzzle") , *_url, MailId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetPuzzleMail);

	// 요청 전송
	Request->ProcessRequest();
}

// 퍼즐 우편 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetPuzzleMail(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if (Response->GetResponseCode() == 200)
		{
			// JSON 응답 파싱
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			if (FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					int32 MailId = ResponseObject->GetIntegerField(TEXT("mailId"));
					FString Subject = ResponseObject->GetStringField(TEXT("subject"));
					FString Content = ResponseObject->GetStringField(TEXT("content"));
					FString MailCategory = ResponseObject->GetStringField(TEXT("mailCategory"));
					int32 Rank = ResponseObject->GetIntegerField(TEXT("rank"));

					UE_LOG(LogTemp , Log , TEXT("MailId: %d") , MailId);
					UE_LOG(LogTemp , Log , TEXT("Subject: %s") , *Subject)
					UE_LOG(LogTemp , Log , TEXT("Content: %s") , *Content);
					UE_LOG(LogTemp , Log , TEXT("MailCategory: %s") , *MailCategory);
					UE_LOG(LogTemp , Log , TEXT("Rank: %d") , Rank);
					
					FTitles NewTitles;
					FStickers NewStickers;

					// 타이틀 정보 처리
					TSharedPtr<FJsonObject> TitleObject = ResponseObject->GetObjectField(TEXT("titleInfo"));
					if (TitleObject.IsValid())
					{
						if (FJsonObjectConverter::JsonObjectToUStruct(TitleObject.ToSharedRef() , &NewTitles))
						{
							UE_LOG(LogTemp , Log , TEXT("Titles Info | Id: %d, Name: %s, Script: %s, Rarity: %s") ,
								   NewTitles.titleId ,
								   *NewTitles.titleName ,
								   *NewTitles.titleScript ,
								   *NewTitles.titleRarity);

							// 기존 TitleItems 배열에 추가
							TitleItems.Add(NewTitles);
						}
					}

					// 스티커 정보 처리
					TSharedPtr<FJsonObject> StickerObject = ResponseObject->GetObjectField(TEXT("stickerInfo"));
					if (StickerObject.IsValid())
					{
						if (FJsonObjectConverter::JsonObjectToUStruct(StickerObject.ToSharedRef() , &NewStickers))
						{
							UE_LOG(LogTemp , Log ,
								   TEXT("Stikers Info | Id: %d, Name: %s, Script: %s, Rarity: %s, StickerImg: %s") ,
								   NewStickers.stickerId ,
								   *NewStickers.stickerName ,
								   *NewStickers.stickerScript ,
								   *NewStickers.stickerRarity ,
								   *NewStickers.stickerImage);

							// 기존 StickerItems 배열에 추가
							StickerItems.Add(NewStickers);
						}
					}
					MainUI->WBP_MH_MainBar->WBP_NoticeUI->OnMailDetailReceived(true, Subject, Content);
					MainUI->WBP_MH_MainBar->WBP_NoticeUI->OnPuzzleTitleStickerReceived(Rank, NewTitles, NewStickers);
					UE_LOG(LogTemp , Log , TEXT("퍼즐 우편 조회 성공"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("퍼즐 우편 조회 실패"));
		}
	}
}

// 커뮤니티홀 나무 조회 요청
void AHM_HttpActor3::ReqGetCommunityTree(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 조회 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/hall/tree") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetCommunityTree);

	// 요청 전송
	Request->ProcessRequest();
}

// 커뮤니티홀 나무 조회 요청에 대한 응답
void AHM_HttpActor3::OnResGetCommunityTree(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					// 메일 목록
					TArray<TSharedPtr<FJsonValue>> TreeList = ResponseObject->GetArrayField(TEXT("ticketTreeDTOList"));
					for ( const TSharedPtr<FJsonValue>& TreeValue : TreeList )
					{
						TSharedPtr<FJsonObject> TreeObject = TreeValue->AsObject();
						if (TreeObject.IsValid())
						{
							// 받아올 정보 추출
							int32 TicketTreeId = TreeObject->GetIntegerField(TEXT("ticketTreeId"));
							FString TicketImg = TreeObject->GetStringField(TEXT("ticketImage"));
							UE_LOG(LogTemp , Log , TEXT("ticketTreeId : %d") , TicketTreeId);
							UE_LOG(LogTemp , Log , TEXT("TicketImg : %s") , *TicketImg);
							
							auto* Tree = Cast<AHM_Tree>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_Tree::StaticClass()));
							if (Tree)
							{
								Tree->InitializeTicketTabs(TicketTreeId, TicketImg);
							}
						}
					}
				}
				UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 조회 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 조회 실패"));
		}
	}
}

// 커뮤니티홀 나무에 달 커스텀 티켓 조회
void AHM_HttpActor3::ReqGetCustomTicketHangOnTree(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("나무에 달 커스텀 티켓 조회"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/tickets/type/custom") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetCustomTicketHangOnTree);

	// 요청 전송
	Request->ProcessRequest();
}

// 커뮤니티홀 나무에 달 커스텀 티켓 조회에 대한 응답
void AHM_HttpActor3::OnResGetCustomTicketHangOnTree(FHttpRequestPtr Request, FHttpResponsePtr Response,
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
				TMap<int32, FString> TicketMap; // 데이터를 저장할 TMap
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					
					// 커스텀 티켓 목록
					TArray<TSharedPtr<FJsonValue>> TreeList = ResponseObject->GetArrayField(TEXT("customTicketDTOList"));
					for ( const TSharedPtr<FJsonValue>& TreeValue : TreeList )
					{
						TSharedPtr<FJsonObject> TreeObject = TreeValue->AsObject();
						if (TreeObject.IsValid())
						{
							// 받아올 정보 추출
							int32 TicketId = TreeObject->GetIntegerField(TEXT("ticketId"));
							FString CustomTicketImg = TreeObject->GetStringField(TEXT("customTicketImage"));
							UE_LOG(LogTemp , Log , TEXT("TicketId : %d") , TicketId);
							UE_LOG(LogTemp , Log , TEXT("CustomTicketImg : %s") , *CustomTicketImg);
							TicketMap.Add(TicketId, CustomTicketImg);
						}
					}
				}
				// 위젯에 데이터 전달
				if (TreeTicketUI)
				{
					TreeTicketUI->InitializeTicketTabs(TicketMap);
					TreeTicketUI->SetVisibility(ESlateVisibility::Visible);
				}
				UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무에 달 커스텀 티켓 조회 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무에 달 커스텀 티켓 조회 실패"));
		}
	}
}

// 커뮤니티홀 나무에 티켓 달기 요청
void AHM_HttpActor3::ReqPostHangingTicketFromTree(int32 TicketId, FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("나무에 티켓 달기 요청"));
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("TicketId: %d"), TicketId);
	FString FormattedUrl = FString::Printf(TEXT("%s/hall/tree/tickets/%d") , *_url, TicketId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResPostHangingTicketFromTree);

	// 요청 전송
	Request->ProcessRequest();
}

// 커뮤니티홀 나무에 티켓 달기 요청에 대한 응답
void AHM_HttpActor3::OnResPostHangingTicketFromTree(FHttpRequestPtr Request, FHttpResponsePtr Response,
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
				if (ResponseObject.IsValid())
				{
					// 받아올 정보 추출
					int32 TicketTreeId = ResponseObject->GetIntegerField(TEXT("ticketTreeId"));
					FString TicketImg = ResponseObject->GetStringField(TEXT("ticketImage"));
					UE_LOG(LogTemp , Log , TEXT("TicketTreeId : %d") , TicketTreeId);
					UE_LOG(LogTemp , Log , TEXT("TicketImg : %s") , *TicketImg);

					ATTPlayerController* PC = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
					ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>();
					if (PC && HallState)
					{
						if(HasAuthority())
						{
							HallState->Multicast_ApplyTicketImage(TicketTreeId, TicketImg);
						}
						else
						{
							PC->ServerRequestApplyTicketImage(TicketTreeId, TicketImg);
						}
					}
				}
				TreeTicketUI->Can_Choose->SetVisibility(ESlateVisibility::Hidden);
				TreeTicketUI->SetVisibility(ESlateVisibility::Hidden);
				UE_LOG(LogTemp , Log , TEXT("나무에 티켓 달기 성공"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("나무에 티켓 달기 실패"));
		}
	}
}

void AHM_HttpActor3::ReqDeleteCommunityTree(FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 티켓 삭제 요청"));
	
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/hall/tree") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor3::OnResGetCommunityTree);
	Request->ProcessRequest();
}

void AHM_HttpActor3::OnResDeleteCommunityTree(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
			auto* Tree = Cast<AHM_Tree>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_Tree::StaticClass()));
			if(!Tree) return;

			for(int32 i = 0; i < Tree->Ticats.Num(); i++)
			{
				Tree->TicatVisibilities[i] = false;
				UE_LOG(LogTemp , Log , TEXT("Tree->TicatVisibilities[%d] = false"), i);
			}
				UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 티켓 삭제 성공"));
		}
		else
		{
			UE_LOG(LogTemp , Log , TEXT("커뮤니티홀 나무 티켓 삭제 실패"));
		}
	}
}


// ================================================================================================================================

// Fill out your copyright notice in the Description page of Project Settings.

