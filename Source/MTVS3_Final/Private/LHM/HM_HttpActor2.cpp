// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"

#include <string>

#include "JMH/MH_TicketingWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "Components/EditableText.h"
#include "JMH/MH_BuyTicketWidget.h"
#include "GameFramework/PlayerState.h"
#include "HJ/TTHallGameState.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_Chair.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "LHM/HM_HttpActor3.h"

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
// 콘서트 정보 요청
void AHM_HttpActor2::ReqGetConcertInfo(FString AccessToken , ATTPlayer* TTPlayer)
{
	TargetPlayer = TTPlayer;

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

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

// 콘서트 정보 요청에 대한 응답
void AHM_HttpActor2::OnResGetConcertInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
				{
					// 콘서트 목록
					TArray<TSharedPtr<FJsonValue>> ConcertList = ResponseObject->GetArrayField(TEXT("concertDTOList"));
					for (const TSharedPtr<FJsonValue>& ConcertValue : ConcertList)
					{
						TSharedPtr<FJsonObject> ConcertObject = ConcertValue->AsObject();

						// JSON 문자열을 FConcertInfo 구조체로 변환
						FConcertInfo NewConcertInfo;
						if (FJsonObjectConverter::JsonObjectToUStruct(ConcertObject.ToSharedRef() , &NewConcertInfo ,
						                                              0 , 0))
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
								MainUI->SetWidgetSwitcher(5);
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
	if (!Http) return;
	FConcertInfo NewConcertInfo;
	UE_LOG(LogTemp , Log , TEXT("GetConcertId: %d") , GetConcertId());
	UE_LOG(LogTemp , Log , TEXT("NewConcertInfo.concertId: %d") , NewConcertInfo.concertId);

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d") , *_url , GetConcertId());
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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
		{
			TargetPlayer->ServerTeleportPlayer(true);
			TargetPlayer->PlayConcertBGM();
			TargetPlayer->MainUI->PlayTitleAnim(2);
			GI->SetPlaceState(EPlaceState::ConcertHall);

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
					// 필요한 정보 추출
					int32 RemainingTickets = ResponseObject->GetIntegerField(TEXT("remainingTickets"));

					// RemainingTickets을 GameInstance에 저장
					TicketingUI->SetTextRemainingTicket(RemainingTickets);
					MainUI->SetTextRemainingTicket(RemainingTickets);
					UE_LOG(LogTemp , Log , TEXT("RemainingTicketCount: %d") , RemainingTickets);

					// 1. FConcertInfo 구조체로 콘서트 정보 파싱
					FConcertInfo NewConcertInfo;
					if (FJsonObjectConverter::JsonObjectToUStruct(ResponseObject.ToSharedRef() , &NewConcertInfo , 0 ,
					                                              0))
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
							MainUI->BuyTicketWidget->SetTextConcertName(NewConcertInfo.concertName);
						}
						if (TicketingUI)
						{
							TicketingUI->SetConcertInfo(NewConcertInfo.concertName , NewConcertInfo.concertTime.year ,
							                            NewConcertInfo.concertTime.month ,
							                            NewConcertInfo.concertTime.day ,
							                            NewConcertInfo.concertTime.time);
						}
					}

					// 2. FSeatsList 구조체로 좌석 정보 파싱
					FSeatsList NewSeatsList;

					// 2-1. FAvailableSeats 접수 가능한 좌석
					TArray<TSharedPtr<FJsonValue>> LocalAvailableSeats = ResponseObject->GetArrayField(
						TEXT("availableSeats"));
					for (const TSharedPtr<FJsonValue>& AvailbleValue : LocalAvailableSeats)
					{
						FAvailableSeats AvailableSeat;
						TSharedPtr<FJsonObject> AvailbleObject = AvailbleValue->AsObject();

						if (FJsonObjectConverter::JsonObjectToUStruct(AvailbleObject.ToSharedRef() , &AvailableSeat ,
						                                              0 , 0))
						{
							NewSeatsList.availableSeats.Add(AvailableSeat);
							UE_LOG(LogTemp , Log , TEXT("Available Seat | ID: %d, Name: %s, DrawingTime: %s")
							       , AvailableSeat.seatId , *AvailableSeat.seatName , *AvailableSeat.drawingTime);

							// 공연장 입장할 때 접수 가능한 좌석 데이터 받아서 의자 액터에 이펙터 처리
						}
					}

					// 2-2. FReservedSeats 예약 완료된 좌석
					TArray<TSharedPtr<FJsonValue>> LocalReceptionSeats = ResponseObject->GetArrayField(
						TEXT("reservedSeats"));
					for (const TSharedPtr<FJsonValue>& ReceptionValue : LocalReceptionSeats)
					{
						FReservedSeats ReservedSeat;
						TSharedPtr<FJsonObject> ReceptionObject = ReceptionValue->AsObject();

						if (FJsonObjectConverter::JsonObjectToUStruct(ReceptionObject.ToSharedRef() , &ReservedSeat ,
						                                              0 , 0))
						{
							NewSeatsList.reservedSeats.Add(ReservedSeat);
							UE_LOG(LogTemp , Log , TEXT("Reception Seat | ID: %d, Name: %s, DrawingTime: %s")
							       , ReservedSeat.seatId , *ReservedSeat.seatName , *ReservedSeat.drawingTime);

							// 공연장 입장할 때 접수 가능한 좌석 데이터 받아서 의자 액터에 이펙터 처리
						}
					}

					// 2-3. FMyReceptionSeats 구조체로 내가 접수한 좌석 정보 파싱
					TArray<TSharedPtr<FJsonValue>> LocalMyReceptionSeat = ResponseObject->GetArrayField(
						TEXT("myReceptionSeats"));
					for (TSharedPtr<FJsonValue> MySeatValue : LocalMyReceptionSeat)
					{
						FMyReceptionSeats MySeatInfo;
						TSharedPtr<FJsonObject> MySeatObject = MySeatValue->AsObject();

						if (FJsonObjectConverter::JsonObjectToUStruct(MySeatObject.ToSharedRef() , &MySeatInfo , 0 , 0))
						{
							NewSeatsList.myReceptionSeats.Add(MySeatInfo);
							// 변환된 NewMySeatInfo 구조체에 대한 디버그 메시지 출력
							UE_LOG(LogTemp , Log , TEXT("My Reception Seat | ID: %d, Name: %s, DrawingTime: %s")
							       , MySeatInfo.seatId , *MySeatInfo.seatName , *MySeatInfo.drawingTime);
							SetReceptionSeatId(MySeatInfo.seatId);
							//GI->SetReceivedSeatId(MySeatInfo.seatId);
						}
					}
					// 우편함에 내가 접수한 좌석 정보 확인하는 UI Switcher & 정보 SetText
					SetSeatsList(NewSeatsList);
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("뉴진스 콘서트 입장~~~")));

					// KHJ: 의자 라이트 업데이트
					// 월드의 모든 MH_Chair 찾기
					TArray<AActor*> FoundChairs;
					UGameplayStatics::GetAllActorsOfClass(GetWorld() , AMH_Chair::StaticClass() , FoundChairs);
					///MH
					TArray<int32> ReservedSeatIndices;

					// Reserved Seats 처리 (예약된 좌석은 false로 설정)
					for (const FReservedSeats& ReservedSeat : NewSeatsList.reservedSeats)
					{
						ReservedSeatIndices.Add(ReservedSeat.seatId);
						
						FString TagToFind = FString::FromInt(ReservedSeat.seatId);
						for (AActor* Actor : FoundChairs)
						{
							if (Actor->ActorHasTag(*TagToFind))
							{
								if (AMH_Chair* Chair = Cast<AMH_Chair>(Actor))
								{
									UE_LOG(LogTemp , Log , TEXT("Found matching chair! Name: %s, Tag: %s") ,
									       *Chair->GetName() , *TagToFind);
									
									// Chair->SetbIsAvailable(false);
									// Chair->OnRep_bIsAvailable();
									// Chair->ChangeLightColor(false);
									
									///MH
									int32 SeatIndex = FCString::Atoi(*TagToFind) - 1; // 태그를 배열 인덱스와 맞추기 위해 1을 뺌
									if (SeatIndex >= 0)
									{
										ReservedSeatIndices.Add(SeatIndex);
									}
									//break;
								}
							}
						}
					}

					// GameState를 통해 한 번에 모든 의자 상태 업데이트
					if (ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>())
					{
						HallState->MulticastUpdateChairStates(ReservedSeatIndices);
					}
					
					///MH
					if (TicketingUI)
					{
						TicketingUI->UpdateReservedSeatsUI(ReservedSeatIndices);
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
#pragma endregion

#pragma region Register seat
// 좌석 조회 요청
void AHM_HttpActor2::ReqGetSeatRegistrationInquiry(FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	UE_LOG(LogTemp , Log , TEXT("GetConcertId: %d") , GetConcertId());
	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s") , *_url , GetConcertId() , *SeatId);
	//FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/1") , *_url, GetConcertId());
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
void AHM_HttpActor2::OnResGetSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response ,
                                                     bool bWasSuccessful)
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
				{
					// 필요한 정보 추출
					int32 Floor = ResponseObject->GetIntegerField(TEXT("floor"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo")); // ex: A1구역 13번
					bool IsReceived = ResponseObject->GetBoolField(TEXT("isReceived"));
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));

					UE_LOG(LogTemp , Log , TEXT("Floor: %d") , Floor);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo: %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("IsReceived: %s") , IsReceived ? TEXT("true") : TEXT("false"));
					UE_LOG(LogTemp , Log , TEXT("CompetitionRate: %d") , CompetitionRate);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice: %d") , SeatPrice);

					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if (GI)
					{
						GI->SetIsReceived(IsReceived);
					}

					if (MainUI) MainUI->SetTextSeatNum1(SeatInfo);

					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextSeatID(Floor , SeatInfo);
					}

					if (TicketingUI)
					{
						TicketingUI->SetTextSeatID(Floor , SeatInfo);
						TicketingUI->SetTextCompetitionRate(CompetitionRate);
						TicketingUI->SetTextTicketPrice(SeatPrice);
					}

					// "drawingTime" 객체를 얻음
					TSharedPtr<FJsonObject> DrawingTimeObject = ResponseObject->GetObjectField(TEXT("drawingTime"));
					if (DrawingTimeObject.IsValid())
					{
						int32 Year = DrawingTimeObject->GetIntegerField(TEXT("year"));
						int32 Month = DrawingTimeObject->GetIntegerField(TEXT("month"));
						int32 Day = DrawingTimeObject->GetIntegerField(TEXT("day"));
						FString Time = DrawingTimeObject->GetStringField(TEXT("time"));

						UE_LOG(LogTemp , Log , TEXT("Drawing Time: %d-%d-%d %s") , Year , Month , Day , *Time);

						TicketingUI->SetTickettingDate(Year , Month , Day);
						TicketingUI->SetTextGameStartTime(Time);
					}
					if (GI->GetIsReceived() == true) // 접수한 좌석일 때 접수취소 버튼
					{
						// MainUI 숨기기
						MainUI->SetVisibleCanvas(false);
						// 좌석 경쟁 UI 표시(테스트용)
						TicketingUI->SetVisibleSwitcher(true , 0);
						TicketingUI->SetCompletedVisible(true);
					}
					else if (GI->GetIsReceived() == false) // 접수한 좌석이 아닐 때 접수하기 버튼
					{
						// MainUI 숨기기
						MainUI->SetVisibleCanvas(false);
						// 좌석 경쟁 UI 표시(테스트용)
						TicketingUI->SetVisibleSwitcher(true , 0);
						TicketingUI->SetCompletedVisible(false);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to get seat registration count: %s") , *Response->GetContentAsString());
		return;
	}
}

// 좌석 접수 요청
void AHM_HttpActor2::ReqGetRegisterSeat(FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl =
		FString::Printf(TEXT("%s/concerts/%d/seats/%s/reception") , *_url , GetConcertId() , *SeatId);
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
				{
					// 받아올 정보 추출
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 RemainingTicket = ResponseObject->GetNumberField(TEXT("remainingTicket"));
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));
					bool IsReceived = ResponseObject->GetBoolField(TEXT("isReceived"));

					UE_LOG(LogTemp , Log , TEXT("seatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("RemainingTicket : %d") , RemainingTicket);
					UE_LOG(LogTemp , Log , TEXT("CompetitionRate : %d") , CompetitionRate);
					UE_LOG(LogTemp , Log , TEXT("IsReceived: %s") , IsReceived ? TEXT("true") : TEXT("false"));

					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if (GI)
					{
						GI->SetIsReceived(IsReceived);
					}

					if (MainUI)
					{
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
					}

					if (TicketingUI)
					{
						TicketingUI->SetTextRemainingTicket(RemainingTicket);
						TicketingUI->SetTextTicketPrice(SeatPrice);
						TicketingUI->SetTextCompetitionRate(CompetitionRate);
						TicketingUI->SetCompletedVisible(true);
					}
					ReqGetMyRegisteredSeat(GI->GetAccessToken()); // 내가 접수한 좌석 조회 요청
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
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/receptions") , *_url , GetConcertId());
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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					// "receptionSeatDTOList" 가져옴
					TArray<TSharedPtr<FJsonValue>> MyReceptionSeat = ResponseObject->GetArrayField(
						TEXT("receptionSeatDTOList"));

					// 배열 내의 각 객체를 처리
					for (TSharedPtr<FJsonValue> MySeatValue : MyReceptionSeat)
					{
						TSharedPtr<FJsonObject> MySeatObject = MySeatValue->AsObject();

						// JSON 문자열을 FMyReceptionSeatInfo 구조체로 변환
						FReceptionSeats NewMySeatInfo;
						if (FJsonObjectConverter::JsonObjectToUStruct(MySeatObject.ToSharedRef() , &NewMySeatInfo , 0 ,
						                                              0))
						{
							SetReceptionSeats(NewMySeatInfo);
							// 변환된 NewMySeatInfo 구조체에 대한 디버그 메시지 출력
							UE_LOG(LogTemp , Log ,
							       TEXT("My Reception Seat Info: %d | %s | %s | %d | %d | %d | %s | %d") ,
							       NewMySeatInfo.seatId ,
							       *NewMySeatInfo.seatName ,
							       *NewMySeatInfo.seatInfo ,
							       NewMySeatInfo.drawingTime.year ,
							       NewMySeatInfo.drawingTime.month ,
							       NewMySeatInfo.drawingTime.day ,
							       *NewMySeatInfo.drawingTime.time ,
							       NewMySeatInfo.competitionRate);
							//GI->SetReceivedSeatId(NewMySeatInfo.seatId);
						}
					}
					// 우편함에 내가 접수한 좌석 정보 확인하는 UI Switcher & 정보 SetText
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

// 좌석 취소 요청1
void AHM_HttpActor2::ReqDeleteCancelRegisteredSeat(FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s") , *_url , GetConcertId() , *SeatId);
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

// 좌석 취소 요청에 대한 응답1
void AHM_HttpActor2::OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response ,
                                                     bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					int32 RemainingTicket = ResponseObject->GetIntegerField(TEXT("remainingTicket"));
					bool IsReceived = ResponseObject->GetBoolField(TEXT("isReceived"));
					UE_LOG(LogTemp , Log , TEXT("IsReceived: %s") , IsReceived ? TEXT("true") : TEXT("false"));

					if (GI)
					{
						GI->SetIsReceived(IsReceived);
					}

					if (GI && TicketingUI)
					{
						// 접수 취소 성공했을 때
						TicketingUI->SetTextRemainingTicket(RemainingTicket);
						TicketingUI->SetVisibleSwitcher(true , 0);
						TicketingUI->SetCompletedVisible(false);
					}
				}
			}
		}
		else 
		{
			if (Response.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get seat registration. Response code: %d, Content: %s"), 
					   Response->GetResponseCode(), *Response->GetContentAsString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get seat registration: Response code error"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get seat registration: Request failed or invalid response"));
	}
}

// 좌석 취소 요청2
void AHM_HttpActor2::ReqDeleteCancelRegisteredSeat2(FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s") , *_url , GetConcertId() , *SeatId);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResDeleteCancelRegisteredSeat2);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 취소 요청에 대한 응답2
void AHM_HttpActor2::OnResDeleteCancelRegisteredSeat2(FHttpRequestPtr Request , FHttpResponsePtr Response ,
                                                      bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					int32 RemainingTicket = ResponseObject->GetIntegerField(TEXT("remainingTicket"));
					bool IsReceived = ResponseObject->GetBoolField(TEXT("isReceived"));
					UE_LOG(LogTemp , Log , TEXT("IsReceived: %s") , IsReceived ? TEXT("true") : TEXT("false"));

					if (GI)
					{
						GI->SetIsReceived(IsReceived);
					}

					if (GI && TicketingUI)
					{
						// 접수 취소 성공했을 때
						TicketingUI->SetTextRemainingTicket(RemainingTicket);
						TicketingUI->SetVisibleSwitcher(false , 1);
					}
					if (MainUI) MainUI->SetVisibleCanvas(true);
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
void AHM_HttpActor2::ReqPostNoticeGameStart(FString SeatId , FString AccessToken)
{
	if (AccessToken == TEXT("-1")) return;
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s/drawing") , *_url, GetConcertId(), *SeatId);
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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		// 캐스팅은 여기서 한 번만 실행
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		APlayerState* PS = Cast<APlayerState>(GetWorld()->GetFirstPlayerController()->GetPlayerState<APlayerState>());

		if (Response->GetResponseCode() == 200)
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
					// nicknameList 파싱
					TArray<FString> NicknameList;
					const TArray<TSharedPtr<FJsonValue>>* NicknameArray;
					if (ResponseObject->TryGetArrayField(TEXT("nicknameList") , NicknameArray))
					{
						for (const TSharedPtr<FJsonValue>& Value : *NicknameArray)
						{
							NicknameList.Add(Value->AsString());
						}
						UE_LOG(LogTemp , Log , TEXT("Received %d nicknames") , NicknameList.Num());

						// NicknameList의 모든 요소를 ", "로 구분하여 출력
						FString NicknameListString = FString::Join(NicknameList , TEXT(", "));
						UE_LOG(LogTemp , Log , TEXT("NicknameList: %s") , *NicknameListString);
					}

					// competitionRate 파싱
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));
					UE_LOG(LogTemp , Log , TEXT("Competition Rate: %d") , CompetitionRate);

					TicketingUI->SetTextCompetitionRate(CompetitionRate);

					ATTHallGameState* HallGameState = GetWorld()->GetGameState<ATTHallGameState>();
					if (HallGameState) HallGameState->SendLuckyDrawInvitation(NicknameList , CompetitionRate);
					UE_LOG(LogTemp , Log , TEXT("추첨 시작 알림 요청 성공"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to get concert entry: %s") , *Response->GetContentAsString());
		}
	}
}

// 좌석 추첨 결과 요청
void AHM_HttpActor2::ReqPostGameResult(FString SeatId , FString AccessToken)
{
	UE_LOG(LogTemp , Log , TEXT("Request Post Game Result"));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	UE_LOG(LogTemp , Log , TEXT("GetConcertId(): %d") , GetConcertId());
	UE_LOG(LogTemp , Log , TEXT("SeatId: %s") , *SeatId);

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/seats/%s/result") , *_url, GetConcertId(), *SeatId);
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

// 좌석 추첨 결과 요청에 대한 응답인데 일단 당첨자, 탈락자 클라이언트에서 처리해줘서 필요없을지도
void AHM_HttpActor2::OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
				{
					UE_LOG(LogTemp , Log , TEXT("Request Post Game Result successful!"));
					// 추첨이 끝났을 때 당첨자에게만 예매창 UI 띄우는 함수에 연결
					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetWidgetSwitcher(0);
					}
				}
			}
		}
		else if (Response->GetResponseCode() == 400)
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
	if (!Http) return;

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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if (Response->GetResponseCode() == 200) // 성공적 응답 (코드 200)
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
					FString _UserCode = ResponseObject->GetStringField(TEXT("userCode"));

					SetUserCode(_UserCode);

					// Base64로 인코딩된 이미지 데이터 추출
					FString Base64Image = ResponseObject->GetStringField(TEXT("image"));
					TArray<uint8> ImageData;
					FBase64::Decode(Base64Image , ImageData);

					// 텍스처로 변환
					UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
					if (Texture)
					{
						if (MainUI->GetBuyTicketWidget())
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
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/qr/verification/success") , *_url);
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
	Writer->WriteValue(TEXT("concertId") , GetConcertId());
	Writer->WriteValue(TEXT("seatId") , GI->GetLuckyDrawSeatID());
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
void AHM_HttpActor2::OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response ,
                                                    bool bWasSuccessful)
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
				{
					// 받아올 정보 추출
					int32 Floor = ResponseObject->GetIntegerField(TEXT("floor"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					UE_LOG(LogTemp , Log , TEXT("Floor : %d / SeatInfo : %s / SeatNum : %d / SeatPrice : %d") , Floor , *SeatInfo ,
					       SeatNum , SeatPrice);

					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextSeatID(Floor , SeatInfo);
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(1);
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
						UE_LOG(LogTemp , Log , TEXT("Member authentication was successful!"));
					}
				}
			}
		}
		else if (Response->GetResponseCode() == 400)
		{
			if (MainUI->GetBuyTicketWidget())
			{
				MainUI->BuyTicketWidget->SetWidgetSwitcher(2);
				UE_LOG(LogTemp , Log , TEXT("Member authentication failed!"));
			}
		}
	}
}

// 예매자 정보 입력 요청
void AHM_HttpActor2::ReqPostReservationinfo(FText UserName , FText UserPhoneNum , FText UserAddress1 ,
                                            FText UserAddress2 , FString AccessToken)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(
		TEXT("%s/concerts/%d/seats/%s/reservation") , *_url , GetConcertId() , *GI->GetLuckyDrawSeatID());
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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					// 필요한 정보 추출
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 UserCoin = ResponseObject->GetIntegerField(TEXT("userCoin"));
					int32 NeededCoin = ResponseObject->GetIntegerField(TEXT("neededCoin"));

					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("Coin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("NeededCoin : %d") , NeededCoin);

					if (TTPlayer && GI)
					{
						GI->SetCoin(UserCoin);
					}
					//SetSeatInfo(SeatInfo);
					//SetSeatPrice(SeatPrice);

					// 결제 진행하는 위젯스위쳐 Set
					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
						int32 TotalCoin = SeatNum * SeatPrice;
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
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(
		TEXT("%s/concerts/%d/seats/%s/payment") , *_url , GetConcertId() , *GI->GetLuckyDrawSeatID());
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
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					// 필요한 정보 추출
					FString SeatId = ResponseObject->GetStringField(TEXT("seatId"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatNum = ResponseObject->GetIntegerField(TEXT("seatNum"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 UserCoin = ResponseObject->GetIntegerField(TEXT("userCoin"));
					FString UserName = ResponseObject->GetStringField(TEXT("userName"));
					FString UserPhoneNum = ResponseObject->GetStringField(TEXT("userPhoneNumber"));
					FString UserAddress = ResponseObject->GetStringField(TEXT("userAddress"));
					int32 TicketId = ResponseObject->GetIntegerField(TEXT("ticketId"));

					UE_LOG(LogTemp , Log , TEXT("SeatId : %s") , *SeatId);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("UserCoin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("UserName : %s") , *UserName);
					UE_LOG(LogTemp , Log , TEXT("UserPhoneNum : %s") , *UserPhoneNum);
					UE_LOG(LogTemp , Log , TEXT("UserAddress : %s") , *UserAddress);
					UE_LOG(LogTemp , Log , TEXT("TicketId : %d") , TicketId);

					if (TTPlayer && GI)
					{
						GI->SetCoin(UserCoin);
						GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
					}

					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextUserName(UserName);
						MainUI->BuyTicketWidget->SetWidgetSwitcher(8);
					}

					AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
						UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
					if (HttpActor3)
					{
						HttpActor3->SetTicketId(TicketId);
						UE_LOG(LogTemp , Log , TEXT("TicketId: %d") , TicketId);
					}

					// KHJ: 결제 완료된 좌석 처리
					TArray<int32> ReservedSeatIndices;
        
					// 결제된 좌석 찾기
					TArray<AActor*> FoundChairs;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMH_Chair::StaticClass(), FoundChairs);

					FString TagToFind = SeatId;
					UE_LOG(LogTemp, Log, TEXT("Payment completed - Looking for Chair with ID: %s"), *TagToFind);
					
					for (AActor* Actor : FoundChairs)
					{
						if (Actor->ActorHasTag(*TagToFind))
						{
							if (AMH_Chair* Chair = Cast<AMH_Chair>(Actor))
							{
								UE_LOG(LogTemp, Log, TEXT("Found matching chair for payment! Name: %s, Tag: %s"),
									   *Chair->GetName(), *TagToFind);
                    
								int32 SeatIndex = FCString::Atoi(*TagToFind) - 1; // 태그를 배열 인덱스와 맞추기 위해 1을 뺌
								if (SeatIndex >= 0)
								{
									ReservedSeatIndices.Add(SeatIndex);
								}
							}
						}
					}
					
					// GameState를 통해 한 번에 모든 의자 상태 업데이트
					if (ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>())
					{
						HallState->MulticastUpdateChairStates(ReservedSeatIndices);
					}
					
					///MH
					if (TicketingUI)
					{
						TicketingUI->UpdateReservedSeatsUI(ReservedSeatIndices);
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
	if (!Http) return;

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
void AHM_HttpActor2::OnResGetReservationInfo(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		if (Response->GetResponseCode() == 200)
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
					// 필요한 정보 추출
					FString UserName = ResponseObject->GetStringField(TEXT("userName"));
					FString UserPhoneNum = ResponseObject->GetStringField(TEXT("userPhoneNumber"));
					FString UserAddress1 = ResponseObject->GetStringField(TEXT("userAddress1"));
					FString UserAddress2 = ResponseObject->GetStringField(TEXT("userAddress2"));

					FText usename = FText::FromString(UserName);
					FText phonenumber = FText::FromString(UserPhoneNum);
					FText address1 = FText::FromString(UserAddress1);
					FText address2 = FText::FromString(UserAddress2);

					UE_LOG(LogTemp , Log , TEXT("UserName : %s") , *UserName);
					UE_LOG(LogTemp , Log , TEXT("UserPhoneNum : %s") , *UserPhoneNum);
					UE_LOG(LogTemp , Log , TEXT("UserAddress1 : %s") , *UserAddress1);
					UE_LOG(LogTemp , Log , TEXT("UserAddress2 : %s") , *UserAddress2);

					// 배송지 정보 불러오기
					if (MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->EText_Name->SetText(usename);
						MainUI->BuyTicketWidget->EText_PhoneNum->SetText(phonenumber);
						MainUI->BuyTicketWidget->EText_Address1->SetText(address1);
						MainUI->BuyTicketWidget->EText_Address2->SetText(address2);
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

#pragma region 개발자 키
void AHM_HttpActor2::ReqPostCheatGameResult(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/draw-cheat") , *_url , GetConcertId());
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
	if (!Http) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concerts/%d/payment-cheat") , *_url , GetConcertId());
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

void AHM_HttpActor2::OnResPostCheatPaymentSeat(FHttpRequestPtr Request , FHttpResponsePtr Response ,
                                               bool bWasSuccessful)
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
				// "response" 객체에 접근
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));

				if (ResponseObject.IsValid())
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
					int32 TicketId = ResponseObject->GetIntegerField(TEXT("ticketId"));

					UE_LOG(LogTemp , Log , TEXT("SeatId : %s") , *SeatId);
					UE_LOG(LogTemp , Log , TEXT("SeatInfo : %s") , *SeatInfo);
					UE_LOG(LogTemp , Log , TEXT("SeatNum : %d") , SeatNum);
					UE_LOG(LogTemp , Log , TEXT("SeatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("UserCoin : %d") , UserCoin);
					UE_LOG(LogTemp , Log , TEXT("UserName : %s") , *UserName);
					UE_LOG(LogTemp , Log , TEXT("UserPhoneNum : %s") , *UserPhoneNum);
					UE_LOG(LogTemp , Log , TEXT("UserAddress : %s") , *UserAddress);
					UE_LOG(LogTemp , Log , TEXT("TicketId : %d") , TicketId);

					AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
						UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
					if (HttpActor3)
					{
						HttpActor3->SetTicketId(TicketId);
						UE_LOG(LogTemp , Log , TEXT("TicketId: %d") , TicketId);
					}

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if (TTPlayer && GI)
					{
						GI->SetCoin(UserCoin);
					}
					if (MainUI->GetBuyTicketWidget())
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
