// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"
#include "JMH/MH_TicketingWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "JMH/MH_BuyTicketWidget.h"
#include "GameFramework/PlayerState.h"
#include "HJ/TTHallGameState.h"
#include "JMH/MainWidget.h"

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
	
}

// Called every frame
void AHM_HttpActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 전달받은 MainUI 참조 저장
void AHM_HttpActor2::SetMainUI(UMainWidget* InMainUI)
{
	MainUI = InMainUI;
}

// 전달받은 TicketingUI 참조 저장
void AHM_HttpActor2::SetTicketingUI(UMH_TicketingWidget* InTicketingUI)
{
	TicketingUI = InTicketingUI;
}


#pragma region Entry Concert
// 공연장 선택 UI 요청
void AHM_HttpActor2::ReqGetConcertInfo(FString AccessToken, ATTPlayer* TTPlayer)
{
	TargetPlayer = TTPlayer;

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetConcertInfo);

	// 요청 전송
	Request->ProcessRequest();
}

// 공연장 선택 UI 요청에 대한 응답
void AHM_HttpActor2::OnResGetConcertInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					// 콘서트 목록
					TArray<TSharedPtr<FJsonValue>> ConcertList = ResponseObject->GetArrayField(TEXT("concertDTOList"));
					for ( const TSharedPtr<FJsonValue>& ConcertValue : ConcertList )
					{
						TSharedPtr<FJsonObject> ConcertObject = ConcertValue->AsObject();
						
						// ConcertObject를 JSON 문자열로 변환
						FString ConcertJsonString;
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ConcertJsonString);
						FJsonSerializer::Serialize(ConcertObject.ToSharedRef() , Writer);

						// JSON 문자열을 FConcertInfo 구조체로 변환
						FConcertInfo NewConcertInfo;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(ConcertJsonString , &NewConcertInfo , 0 , 0))
						{
							SetConcertInfo(NewConcertInfo);
							
							// 변환된 NewConcertInfo 구조체에 대한 디버그 메시지 출력
							UE_LOG(LogTemp , Log , TEXT("Concert Info | Id: %d, Name: %s, Date: %d-%d-%d %s") ,
							       NewConcertInfo.concertId ,
							       *NewConcertInfo.concertName ,
							       NewConcertInfo.concertTime.year ,
							       NewConcertInfo.concertTime.month ,
							       NewConcertInfo.concertTime.day ,
							       *NewConcertInfo.concertTime.time);

							if (MainUI && MainUI->GetBuyTicketWidget())
							{
								MainUI->BuyTicketWidget->SetConcertInfo_BuyTicket(
									NewConcertInfo.concertName ,
									NewConcertInfo.concertTime.year ,
									NewConcertInfo.concertTime.month ,
									NewConcertInfo.concertTime.day ,
									NewConcertInfo.concertTime.time);
							}
						}
						else
						{
							UE_LOG(LogTemp , Warning , TEXT("FConcertInfo 구조체로 JSON 변환 실패."));
						}
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

// 공연장 입장 요청
void AHM_HttpActor2::ReqGetConcertEntry(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;
	
	UE_LOG(LogTemp , Log , TEXT("GetConcertId: %d"), GetConcertId());
		
	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d") , *_url, GetConcertId());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetConcertEntry);

	// 요청 전송
	Request->ProcessRequest();
}

// 공연장 입장 요청에 대한 응답
void AHM_HttpActor2::OnResGetConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if ( Response->GetResponseCode() == 200 )
		{
			TargetPlayer->ServerTeleportPlayer(true);
			GI->SetPlaceState(EPlaceState::ConcertHall);
			
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
					// 필요한 정보 추출
					int32 RemainingTickets = ResponseObject->GetIntegerField(TEXT("remainingTickets"));
					
					// RemainingTickets을 GameInstance에 저장
					GI->SetRemainingTicketCount(RemainingTickets);
					TicketingUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
					MainUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
					UE_LOG(LogTemp, Log, TEXT("RemainingTicketCount: %d"), GI->GetRemainingTicketCount());

					// 1. FConcertInfo
					FString ConcertJsonString;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ConcertJsonString);
					FJsonSerializer::Serialize(ResponseObject.ToSharedRef() , Writer);
					
					// JSON 문자열을 FConcertInfo 구조체로 변환
					FConcertInfo NewConcertInfo;
					if (FJsonObjectConverter::JsonObjectStringToUStruct(ConcertJsonString , &NewConcertInfo , 0 , 0))
					{
						SetConcertInfo(NewConcertInfo);
						
						// 변환된 NewConcertInfo 구조체에 대한 디버그 메시지 출력
						UE_LOG(LogTemp , Log , TEXT("Concert Info | Id: %d, Name: %s, Date: %d-%d-%d %s") ,
								NewConcertInfo.concertId ,
							   *NewConcertInfo.concertName ,
							   NewConcertInfo.concertTime.year ,
							   NewConcertInfo.concertTime.month ,
							   NewConcertInfo.concertTime.day ,
							   *NewConcertInfo.concertTime.time);
					}

					// 2. FSeatsList 구조체로 좌석 정보 파싱
					FSeatsList NewSeatsList;
					
					TArray<TSharedPtr<FJsonValue>> LocalAvailableSeats = ResponseObject->GetArrayField(TEXT("availableSeats"));
					for ( const TSharedPtr<FJsonValue>& AvailbleValue : LocalAvailableSeats )
					{
						FAvailableSeats AvailableSeat;
						TSharedPtr<FJsonObject> AvailbleObject = AvailbleValue->AsObject();
						
						// ConcertObject를 JSON 문자열로 변환
						FString AvailbleJsonString;
						TSharedRef<TJsonWriter<>> AvailableWriter  = TJsonWriterFactory<>::Create(&AvailbleJsonString);
						FJsonSerializer::Serialize(AvailbleObject.ToSharedRef() , AvailableWriter);

						if (FJsonObjectConverter::JsonObjectStringToUStruct(AvailbleJsonString , &AvailableSeat , 0 , 0))
						{
							NewSeatsList.availableSeats.Add(AvailableSeat);
							UE_LOG(LogTemp , Log , TEXT("Available Seat | ID: %d, Name: %s, DrawingTime: %s")
										,AvailableSeat.seatId, *AvailableSeat.seatName, *AvailableSeat.drawingTime);
							
							// 공연장 입장할 때 접수 가능한 좌석 데이터 받아서 의자 액터에 이펙터 처리
						}
					}

					TArray<TSharedPtr<FJsonValue>> LocalReceptionSeats = ResponseObject->GetArrayField(TEXT("receptionSeats"));
					for ( const TSharedPtr<FJsonValue>& ReceptionValue : LocalReceptionSeats )
					{
						FReceptionSeats ReceptionSeat;
						TSharedPtr<FJsonObject> ReceptionObject = ReceptionValue->AsObject();
						
						// ConcertObject를 JSON 문자열로 변환
						FString ReceptionJsonString;
						TSharedRef<TJsonWriter<>> ReceptionWriter  = TJsonWriterFactory<>::Create(&ReceptionJsonString);
						FJsonSerializer::Serialize(ReceptionObject.ToSharedRef() , ReceptionWriter);

						if (FJsonObjectConverter::JsonObjectStringToUStruct(ReceptionJsonString , &ReceptionSeat , 0 , 0))
						{
							NewSeatsList.receptionSeats.Add(ReceptionSeat);
							UE_LOG(LogTemp , Log , TEXT("Reception Seat | ID: %d, Name: %s, DrawingTime: %s")
										,ReceptionSeat.seatId, *ReceptionSeat.seatName, *ReceptionSeat.drawingTime);
							
							// 공연장 입장할 때 접수 가능한 좌석 데이터 받아서 의자 액터에 이펙터 처리
						}
					}
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("뉴진스 콘서트 입장~~~")));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get concert entry: %s"), *Response->GetContentAsString());
		}
	}
}
#pragma endregion

#pragma region Register seat
// 좌석 조회 요청
void AHM_HttpActor2::ReqGetSeatRegistrationInquiry(FString SeatName , FString AccessToken)
{

	// ChairTag랑 FAvailableSeats의 SeatName과 비교해서 같으면 해당 SeatName의 SeatId를 매개변수로 받아서 url에 Set해주기
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	UE_LOG(LogTemp , Log , TEXT("GetConcertId: %d") , GetConcertId());
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1") , *_url, GetConcertId()); // 임의 SeatId(1) SeatId => SeatName으로 바꿀 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetSeatRegistrationInquiry);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 조회 요청에 대한 응답
void AHM_HttpActor2::OnResGetSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					// 필요한 정보 추출
					int32 Floor = ResponseObject->GetIntegerField(TEXT("floor"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo")); // ex: A1구역 13번
					bool IsReceived = ResponseObject->GetBoolField(TEXT("isReceived"));
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));
					
					UE_LOG(LogTemp , Log , TEXT("Floor: %d") , Floor);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo: %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("IsReceived: %s") , IsReceived ? TEXT("true") : TEXT("false"));
					UE_LOG(LogTemp , Log , TEXT("CompetitionRate: %d") , CompetitionRate);

					if(MainUI) MainUI->SetTextSeatNum1(SeatInfo);
					
					if(MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextSeatID(Floor, SeatInfo);
					}
					
					if ( TicketingUI )
					{
						TicketingUI->SetTextSeatID(Floor , SeatInfo);
						TicketingUI->SetTextCompetitionRate(CompetitionRate);
					}

					// "drawingTime" 객체를 얻음
					TSharedPtr<FJsonObject> DrawingTimeObject = ResponseObject->GetObjectField(TEXT("drawingTime"));
					if ( DrawingTimeObject.IsValid() )
					{
						int32 Year = DrawingTimeObject->GetIntegerField(TEXT("year"));
						int32 Month = DrawingTimeObject->GetIntegerField(TEXT("month"));
						int32 Day = DrawingTimeObject->GetIntegerField(TEXT("day"));
						FString Time = DrawingTimeObject->GetStringField(TEXT("time"));

						UE_LOG(LogTemp , Log , TEXT("Drawing Time: %d-%d-%d %s") , Year , Month , Day , *Time);

						TicketingUI->SetTickettingDate(Year , Month , Day);
						TicketingUI->SetTextGameStartTime(Time);
					}

				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to get seat registration count: %s") , *Response->GetContentAsString());
	}
}

// 좌석 접수 요청
void AHM_HttpActor2::ReqGetRegisterSeat(FString SeatName , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1/reception") , *_url, GetConcertId()); // 임의 SeatId(1) SeatId => SeatName으로 바꿀 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetRegisterSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 접수 요청에 대한 응답
void AHM_HttpActor2::OnResGetRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

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
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 RemainingTicket = ResponseObject->GetNumberField(TEXT("remainingTicket"));
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));

					UE_LOG(LogTemp , Log , TEXT("seatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("RemainingTicket : %d") , RemainingTicket);
					UE_LOG(LogTemp , Log , TEXT("CompetitionRate : %d") , CompetitionRate);

					if( GI && MainUI )
					{
						GI->SetRemainingTicketCount(RemainingTicket);
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
					}

					if ( TicketingUI )
					{
						TicketingUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
						TicketingUI->SetTextTicketPrice(SeatPrice);
						TicketingUI->SetTextCompetitionRate(CompetitionRate);
						TicketingUI->SetCompletedVisible(true);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to register seat: %s") , *Response->GetContentAsString());
	}
}

// 내가 접수한 좌석 조회 요청
void AHM_HttpActor2::ReqGetMyRegisteredSeat(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/receptions") , *_url, GetConcertId());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetMyRegisteredSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 내가 접수한 좌석 조회 요청 응답
void AHM_HttpActor2::OnResGetMyRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

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
					// "receptionSeatDTOList" 가져옴
					TArray<TSharedPtr<FJsonValue>> MyReceptionSeat = ResponseObject->GetArrayField(TEXT("receptionSeatDTOList"));

					// 배열 내의 각 객체를 처리
					for ( TSharedPtr<FJsonValue> MySeatValue : MyReceptionSeat )
					{
						TSharedPtr<FJsonObject> MySeatObject = MySeatValue->AsObject();
						
						// MySeatObject를 JSON 문자열로 변환
						FString MySeatJsonString;
						TSharedRef<TJsonWriter<>> MySeatWriter  = TJsonWriterFactory<>::Create(&MySeatJsonString);
						FJsonSerializer::Serialize(MySeatObject.ToSharedRef() , MySeatWriter);

						// JSON 문자열을 FMyReceptionSeatInfo 구조체로 변환
						FMyReceptionSeats NewMySeatInfo;
						if (FJsonObjectConverter::JsonObjectStringToUStruct(MySeatJsonString , &NewMySeatInfo , 0 , 0))
						{
							SetMyReceptionSeats(NewMySeatInfo);
							// 변환된 NewMySeatInfo 구조체에 대한 디버그 메시지 출력
							UE_LOG(LogTemp , Log , TEXT("My Reception Seat Info: %d | %s | %s | %d | %d | %d | %s | %d") ,
								   NewMySeatInfo.seatId ,
								   *NewMySeatInfo.seatName ,
								   *NewMySeatInfo.seatInfo ,
								   NewMySeatInfo.year ,
								   NewMySeatInfo.month ,
								   NewMySeatInfo.day ,
								   *NewMySeatInfo.time ,
								   NewMySeatInfo.competitionRate);
						}
					}
					// 알림창(?)에 내가 접수한 좌석 확인하는 UI Switcher
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("내가 접수한 좌석")));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to get concert entry: %s") , *Response->GetContentAsString());
		}
	}
}

// 좌석 취소 요청
void AHM_HttpActor2::ReqDeleteCancelRegisteredSeat(FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	//FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%d") , *_url, GetConcertId(), GetMySeatId());
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1") , *_url, GetConcertId()); // 임의 seatId 1 SeatId => SeatName으로 변경?
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResDeleteCancelRegisteredSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 취소 요청에 대한 응답
void AHM_HttpActor2::OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

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
					int32 RemainingTicket = ResponseObject->GetIntegerField(TEXT("remainingTicket"));
					
					if ( GI && TicketingUI )
					{
						// 접수 취소 성공했을 때
						GI->SetRemainingTicketCount(RemainingTicket);
						TicketingUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
						TicketingUI->SetCompletedVisible(false);
						TicketingUI->SetWidgetSwitcher(0);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to cancel seat: %s") , *Response->GetContentAsString());
	}
}
#pragma endregion

#pragma region Lucky Drawing
// 추첨 시작 알림 요청
void AHM_HttpActor2::ReqPostNoticeGameStart(FString SeatName , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1/drawing") , *_url, GetConcertId()); // 임의 SeatId(1) SeatID => SeatName으로 변경 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostNoticeGameStart);

	// 요청 전송
	Request->ProcessRequest();
}

// 추첨 시작 알림 요청에 대한 응답
void AHM_HttpActor2::OnResPostNoticeGameStart(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		APlayerState* PS = Cast<APlayerState>( GetWorld()->GetFirstPlayerController()->GetPlayerState<APlayerState>());

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
					// nicknameList 파싱
					TArray<FString> NicknameList;
					const TArray<TSharedPtr<FJsonValue>>* NicknameArray;
					if ( ResponseObject->TryGetArrayField(TEXT("nicknameList") , NicknameArray) )
					{
						for ( const TSharedPtr<FJsonValue>& Value : *NicknameArray )
						{
							NicknameList.Add(Value->AsString());
						}
						UE_LOG(LogTemp , Log , TEXT("Received %d nicknames") , NicknameList.Num());
						
						// NicknameList의 모든 요소를 ", "로 구분하여 출력
						FString NicknameListString = FString::Join(NicknameList, TEXT(", "));
						UE_LOG(LogTemp, Log, TEXT("NicknameList: %s"), *NicknameListString);
					}

					// competitionRate 파싱
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));
					UE_LOG(LogTemp , Log , TEXT("Competition Rate: %d") , CompetitionRate);

					TicketingUI->SetTextCompetitionRate(CompetitionRate);

					ATTHallGameState* HallGameState = GetWorld()->GetGameState<ATTHallGameState>();
					if (HallGameState) HallGameState->SendLuckyDrawInvitation(NicknameList, CompetitionRate);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to get concert entry: %s") , *Response->GetContentAsString());
		}
	}
}

// 좌석 게임 결과 요청
void AHM_HttpActor2::ReqPostGameResult(FString SeatName , FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("Request Post Game Result"));
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1/result") , *_url, GetConcertId()); // 임의 seatId(1) SeatID => 추첨SeatName 변경 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	//Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostGameResult);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 게임 결과 요청에 대한 응답인데 일단 당첨자, 탈락자 클라이언트에서 처리해줘서 필요없을지도
void AHM_HttpActor2::OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					UE_LOG(LogTemp , Log , TEXT("Request Post Game Result successful!"));
					// 추첨이 끝났을 때 당첨자에게만 예매창 UI 띄우는 함수에 연결
					if ( MainUI->GetBuyTicketWidget() )
					{
						MainUI->BuyTicketWidget->SetWidgetSwitcher(0);
					}
				}
			}
		}
		else if ( Response->GetResponseCode() == 400 )
		{
			UE_LOG(LogTemp , Log , TEXT("Request Post Game Result failed!"));
		}
	}
}
#pragma endregion

#pragma region Seat Payment
// 결제시 회원 인증용 QR 요청
void AHM_HttpActor2::ReqGetMemberAuthQR(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/qr/verification") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetMemberAuthQR);

	// 요청 전송
	Request->ProcessRequest();
}

// 결제시 회원 인증용 QR 요청에 대한 응답
void AHM_HttpActor2::OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
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
					FString _UserCode = ResponseObject->GetStringField(TEXT("userCode"));

					SetUserCode(_UserCode);

					// Base64로 인코딩된 이미지 데이터 추출
					FString Base64Image = ResponseObject->GetStringField(TEXT("image"));
					TArray<uint8> ImageData;
					FBase64::Decode(Base64Image , ImageData);

					// 텍스처로 변환
					UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
					if ( Texture )
					{
						if ( MainUI->GetBuyTicketWidget() )
						{
							MainUI->BuyTicketWidget->SetQRImg(Texture);
							MainUI->SetWidgetSwitcher(3);
						}
						else
						{
							UE_LOG(LogTemp , Warning , TEXT("BuyTicketUI is null."));
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

// 결제시 회원 인증 사진 업로드 확인 요청
void AHM_HttpActor2::ReqGetPostConfirmMemberPhoto(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/qr/verification/success") , *_url); // 미정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userCode") , UserCode);
	Writer->WriteObjectEnd();
	Writer->Close();

	// JSON 본문 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetPostConfirmMemberPhoto);

	// 요청 전송
	Request->ProcessRequest();
}

// 결제시 회원 인증 사진 업로드 확인 요청에 대한 응답
void AHM_HttpActor2::OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					int32 Floor = ResponseObject->GetIntegerField(TEXT("floor"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					UE_LOG(LogTemp , Log , TEXT("Floor : %d / SeatInfo : %s / SeatNum : %d"), Floor , *SeatInfo, SeatNum);

					if ( MainUI->GetBuyTicketWidget() )
					{
						MainUI->BuyTicketWidget->SetTextSeatID(Floor, SeatInfo);
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(1);
						UE_LOG(LogTemp , Log , TEXT("Member authentication was successful!"));
					}
				}
			}
		}
		else if ( Response->GetResponseCode() == 400 )
		{
			if ( MainUI->GetBuyTicketWidget() )
			{
				MainUI->BuyTicketWidget->SetWidgetSwitcher(2);
				UE_LOG(LogTemp , Log , TEXT("Member authentication failed!"));
			}
		}
	}
}

// 예매자 정보 입력 요청
void AHM_HttpActor2::ReqPostReservationinfo(FText UserName , FText UserPhoneNum , FText UserAddress1 , FText UserAddress2 , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1/reservation") , *_url, GetConcertId()); // 임의 SeatId(1) => SeatName으로 변경 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userName") , UserName.ToString());
	Writer->WriteValue(TEXT("userPhoneNumber") , UserPhoneNum.ToString());
	Writer->WriteValue(TEXT("userAddress1") , UserAddress1.ToString());
	Writer->WriteValue(TEXT("userAddress2") , UserAddress2.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostReservationinfo);

	// 요청 전송
	Request->ProcessRequest();
}

// 예매자 정보 입력 요청에 대한 응답
void AHM_HttpActor2::OnResPostReservationinfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		
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
					// 필요한 정보 추출
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 UserCoin = ResponseObject->GetIntegerField(TEXT("userCoin"));
					int32 NeededCoin = ResponseObject->GetIntegerField(TEXT("neededCoin"));

					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s"), *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("Coin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("NeededCoin : %d") , NeededCoin);

					if ( TTPlayer && GI )
					{
						GI->SetCoin(UserCoin);
					}

					//SetSeatInfo(SeatInfo);
					//SetSeatPrice(SeatPrice);
					
					// 결제 진행하는 위젯스위쳐 Set
					if ( MainUI->GetBuyTicketWidget() )
					{
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
						int32 TotalCoin = SeatNum*SeatPrice;
						MainUI->BuyTicketWidget->SetTextTotalCoin(TotalCoin);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(7);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Reservation info: %s") , *Response->GetContentAsString());
		}
	}
}
 
// 좌석 결제 요청
void AHM_HttpActor2::ReqPostPaymentSeat(int32 SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1/payment") , *_url, GetConcertId()); // 임의 seatId(1) => SeatName 변경 예정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostPaymentSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 결제 요청에 대한 응답
void AHM_HttpActor2::OnResPostPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		
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
					// 필요한 정보 추출
					FString SeatId = ResponseObject->GetStringField(TEXT("seatId"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 UserCoin = ResponseObject->GetIntegerField(TEXT("userCoin"));
					FString UserName = ResponseObject->GetStringField(TEXT("userName"));
					FString UserPhoneNum = ResponseObject->GetStringField(TEXT("userPhoneNumber"));
					FString UserAddress = ResponseObject->GetStringField(TEXT("userAddress"));

					UE_LOG(LogTemp , Log , TEXT("SeatId : %s") , *SeatId);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("UserCoin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("UserName : %s") , *UserName);
					UE_LOG(LogTemp , Log , TEXT("UserPhoneNum : %s") , *UserPhoneNum);
					UE_LOG(LogTemp , Log , TEXT("UserAddress : %s") , *UserAddress);

					if ( TTPlayer && GI )
					{
						GI->SetCoin(UserCoin);
						GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
					}
					
					if ( MainUI->GetBuyTicketWidget() )
					{
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextUserName(UserName);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(8);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Payment Seat: %s") , *Response->GetContentAsString());
		}
	}
}

// 예매 정보 불러오기 요청
void AHM_HttpActor2::ReqGetReservationInfo(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/member/address") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetReservationInfo);

	// 요청 전송
	Request->ProcessRequest();
}

// 예매 정보 불러오기 요청에 대한 응답
void AHM_HttpActor2::OnResGetReservationInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	
}

#pragma endregion

#pragma region 개발자 키
void AHM_HttpActor2::ReqPostCheatGameResult(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/draw-cheat") , *_url, GetConcertId());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 전송
	Request->ProcessRequest();

	UE_LOG(LogTemp , Log , TEXT("Cheat Seat 좌석 게임 결과 요청"));
}

void AHM_HttpActor2::ReqPostCheatPaymentSeat(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/payment-cheat") , *_url, GetConcertId());
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostCheatPaymentSeat);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResPostCheatPaymentSeat(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					// 필요한 정보 추출
					FString SeatId = ResponseObject->GetStringField(TEXT("seatId"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 UserCoin = ResponseObject->GetIntegerField(TEXT("userCoin"));
					FString UserName = ResponseObject->GetStringField(TEXT("userName"));
					FString UserPhoneNum = ResponseObject->GetStringField(TEXT("userPhoneNumber"));
					FString UserAddress = ResponseObject->GetStringField(TEXT("userAddress"));

					UE_LOG(LogTemp , Log , TEXT("SeatId : %s") , *SeatId);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("UserCoin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("UserName : %s") , *UserName);
					UE_LOG(LogTemp , Log , TEXT("UserPhoneNum : %s") , *UserPhoneNum);
					UE_LOG(LogTemp , Log , TEXT("UserAddress : %s") , *UserAddress);

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if ( TTPlayer && GI )
					{
						GI->SetCoin(UserCoin);
					}
					if ( MainUI->GetBuyTicketWidget() )
					{
						//MainUI->BuyTicketWidget->SetTextSeatID2(SeatInfo);
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextUserName(UserName);
						MainUI->BuyTicketWidget->SetTextUserPhoneNum(UserPhoneNum);
						MainUI->BuyTicketWidget->SetTextUserAddress(UserAddress);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(8);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Payment Seat: %s") , *Response->GetContentAsString());
		}
	}
}
#pragma endregion
