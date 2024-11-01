// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "JMH/MH_TicketingWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"
#include "ImageUtils.h"
#include "JMH/MH_BuyTicketWidget.h"
#include "GameFramework/PlayerState.h"
#include "HLSLTree/HLSLTreeTypes.h"
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

//=========================================================================================================================================

// 공연장 선택 UI 요청
void AHM_HttpActor2::ReqGetConcertInfo(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert") , *_url);
	//FString FormattedUrl = FString::Printf(TEXT("%s/concert/test") , *_url); // 테스트용
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
					// 콘서트 목록
					TArray<TSharedPtr<FJsonValue>> ConcertList = ResponseObject->GetArrayField(TEXT("concertDTOList"));
					for ( const TSharedPtr<FJsonValue>& ConcertValue : ConcertList )
					{
						TSharedPtr<FJsonObject> ConcertObject = ConcertValue->AsObject();
						int32 ConcertId = ConcertObject->GetIntegerField(TEXT("concertId")); // test용
						FString ConcertName = ConcertObject->GetStringField(TEXT("concertName"));
						int32 Year = ConcertObject->GetIntegerField(TEXT("year"));
						int32 Month = ConcertObject->GetIntegerField(TEXT("month"));
						int32 Day = ConcertObject->GetIntegerField(TEXT("day"));
						FString Time = ConcertObject->GetStringField(TEXT("time"));

						UE_LOG(LogTemp , Log , TEXT("Concert : %d | %s | %d-%d-%d-%s") , ConcertId, *ConcertName, Year, Month, Day, *Time);

						FConcertDTO ConcertDTO;
						SetConcertInfo(ConcertId, ConcertName,Year,Month,Day,Time);
						ConcertDTO.SetConcertId(ConcertId);
						ConcertDTO.SetConcertName(ConcertName);
						ConcertDTO.SetConcertYear(Year);
						ConcertDTO.SetConcertMonth(Month);
						ConcertDTO.SetConcertDay(Day);
						ConcertDTO.SetConcertTime(Time);

						if ( TTPlayer && GI )
						{
							GI->SetConcertName(ConcertName);
						}
						
						if ( MainUI->GetBuyTicketWidget() )
						{
							MainUI->BuyTicketWidget->SetConcertInfo_BuyTicket(ConcertName,Year,Month,Day,Time);
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
void AHM_HttpActor2::ReqPostConcertEntry(FString ConcertName , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 본문 데이터 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostConcertEntry);

	// 요청 전송
	Request->ProcessRequest();
}

// 공연장 입장 요청에 대한 응답
void AHM_HttpActor2::OnResPostConcertEntry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					// 필요한 정보 추출
					FString ConcertName = ResponseObject->GetStringField(TEXT("concertName"));
					int32 RemainingTickets = ResponseObject->GetIntegerField(TEXT("remainingTickets"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("ConcertName : %s") , *ConcertName));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("remainingTickets : %d"), RemainingTickets));

					// RemainingTickets을 GameInstance에 저장
					GI->SetRemainingTicketCount(RemainingTickets);
					TicketingUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
					MainUI->SetTextRemainingTicket(GI->GetRemainingTicketCount());
					
					// 접수 가능한 좌석 목록
					TArray<TSharedPtr<FJsonValue>> AvailableSeatsArray = ResponseObject->GetArrayField(TEXT("availableSeats"));
					TArray<FSeatIdDTO> AvailableSeats;

					for ( const TSharedPtr<FJsonValue>& SeatValue : AvailableSeatsArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();
						FString AvailableSeatId = SeatObject->GetStringField(TEXT("seatId"));
						FString AvailableSeatDrawingTime = SeatObject->GetStringField(TEXT("drawingTime"));

						UE_LOG(LogTemp , Log , TEXT("Available Seat ID: %s") , *AvailableSeatId);
						UE_LOG(LogTemp , Log , TEXT("Available Seat Drawing Time: %s") , *AvailableSeatDrawingTime);

						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(AvailableSeatId);
						SeatDTO.SetDrawingTime(AvailableSeatDrawingTime);
						AvailableSeats.Add(SeatDTO);
					}

					// FConcertReservation에 좌석 추가
					SetAvailableSeats(AvailableSeats);

					// 접수 완료된 좌석 목록
					TArray<TSharedPtr<FJsonValue>> ReceptionSeatsArray = ResponseObject->GetArrayField(TEXT("receptionSeats"));
					TArray<FSeatIdDTO> ReceptionSeats;

					for ( const TSharedPtr<FJsonValue>& SeatValue : ReceptionSeatsArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();
						FString ReceptionSeatId = SeatObject->GetStringField(TEXT("seatId"));
						//FString ReceptionSeatDrawingTime = SeatObject->GetStringField(TEXT("drawingTime"));

						UE_LOG(LogTemp , Log , TEXT("Reception Seat ID: %s") , *ReceptionSeatId);
						//UE_LOG(LogTemp , Log , TEXT("Reception Drawing Time: %s") , *ReceptionSeatDrawingTime);

						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(ReceptionSeatId);
						//SeatDTO.SetDrawingTime(ReceptionSeatDrawingTime);
						ReceptionSeats.Add(SeatDTO);
					}

					// FConcertReservation에 좌석 추가
					SetReceptionSeats(ReceptionSeats);

					// 공연장으로 입장. 캐릭터 스폰 함수 호출
					//if ( TTPlayer )
					//{
					//	TTPlayer->
					//}
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

//=========================================================================================================================================

// 리팩토링 테스트용
void AHM_HttpActor2::TESTReqPostConcertEntry( FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	FString FormattedUrl = FString::Printf(TEXT("%s/concert/%d") , *_url, GetConcertId());

	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("GET"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// // 전달 데이터 (JSON)
	// FString ContentString;
	// TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	// Writer->WriteObjectStart();
	// Writer->WriteValue(TEXT("concertName") , ConcertName);
	// Writer->WriteObjectEnd();
	// Writer->Close();
	//
	// // 본문 데이터 설정
	// Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::TESTOnResPostConcertEntry);

	// 요청 전송
	Request->ProcessRequest();
}

// 리팩토링 테스트용
void AHM_HttpActor2::TESTOnResPostConcertEntry(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
					// 필요한 정보 추출
					FString ConcertName = ResponseObject->GetStringField(TEXT("concertName"));
					int32 RemainingTickets = ResponseObject->GetIntegerField(TEXT("remainingTickets"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("ConcertName : %s") , *ConcertName));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("remainingTickets : %d"), RemainingTickets));

					// RemainingTickets을 GameInstance에 저장
					GI->SetRemainingTicketCount(RemainingTickets);
					
					// 접수 가능한 좌석 목록
					TArray<TSharedPtr<FJsonValue>> AvailableSeatsArray = ResponseObject->GetArrayField(TEXT("availableSeats"));
					TArray<FSeatIdDTO> AvailableSeats;

					for ( const TSharedPtr<FJsonValue>& SeatValue : AvailableSeatsArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();
						FString AvailableSeatId = SeatObject->GetStringField(TEXT("seatId"));
						FString AvailableSeatDrawingTime = SeatObject->GetStringField(TEXT("drawingTime"));

						UE_LOG(LogTemp , Log , TEXT("Available Seat ID: %s") , *AvailableSeatId);
						UE_LOG(LogTemp , Log , TEXT("Available Seat Drawing Time: %s") , *AvailableSeatDrawingTime);

						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(AvailableSeatId);
						SeatDTO.SetDrawingTime(AvailableSeatDrawingTime);
						AvailableSeats.Add(SeatDTO);
					}

					// FConcertReservation에 좌석 추가
					SetAvailableSeats(AvailableSeats);

					// 접수 완료된 좌석 목록
					TArray<TSharedPtr<FJsonValue>> ReceptionSeatsArray = ResponseObject->GetArrayField(TEXT("receptionSeats"));
					TArray<FSeatIdDTO> ReceptionSeats;

					for ( const TSharedPtr<FJsonValue>& SeatValue : ReceptionSeatsArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();
						FString ReceptionSeatId = SeatObject->GetStringField(TEXT("seatId"));
						//FString ReceptionSeatDrawingTime = SeatObject->GetStringField(TEXT("drawingTime"));

						UE_LOG(LogTemp , Log , TEXT("Reception Seat ID: %s") , *ReceptionSeatId);
						//UE_LOG(LogTemp , Log , TEXT("Reception Drawing Time: %s") , *ReceptionSeatDrawingTime);

						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(ReceptionSeatId);
						//SeatDTO.SetDrawingTime(ReceptionSeatDrawingTime);
						ReceptionSeats.Add(SeatDTO);
					}

					// FConcertReservation에 좌석 추가
					SetReceptionSeats(ReceptionSeats);

					// 공연장으로 입장. 캐릭터 스폰 함수 호출
					//if ( TTPlayer )
					//{
					//	TTPlayer->
					//}
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

//=========================================================================================================================================

// 좌석 조회 요청
void AHM_HttpActor2::ReqPostSeatRegistrationInquiry(FString ConcertName, FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat") , *_url); // 미정
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("좌석 조회 요청 SeatId : %s"), *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostSeatRegistrationInquiry);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 조회 요청에 대한 응답
void AHM_HttpActor2::OnResPostSeatRegistrationInquiry(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					int32 Floor = ResponseObject->GetIntegerField(TEXT("floor"));
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo")); // ex: A1구역 13번
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));

					// 로그로 출력
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("SeatId: %s") , *SeatId));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Floor: %d") , Floor));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Seat Info: %s") , *SeatInfo));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Competition Rate: %d") , CompetitionRate));

					SetSeatId(SeatId);
					SetSeatInfo(SeatInfo);
					SetSeatFloor(Floor);
					SetCompetitionRate(CompetitionRate);
					
					MainUI->SetTextSeatNum1(SeatInfo);

					if(MainUI->GetBuyTicketWidget())
					{
						MainUI->BuyTicketWidget->SetTextSeatID(Floor, SeatInfo);
						MainUI->BuyTicketWidget->SetTextConcertName(GetConcertName());
					}
					
					// Ticketing UI에 SetText
					if ( TicketingUI )
					{
						TicketingUI->SetTextSeatID(Floor , SeatInfo);
						TicketingUI->SetTextCompetitionRate(CompetitionRate);
					}

					// "concertTime" 객체를 얻음
					TSharedPtr<FJsonObject> ConcertTimeObject = ResponseObject->GetObjectField(TEXT("concertTime"));
					if ( ConcertTimeObject.IsValid() )
					{
						int32 Year = ConcertTimeObject->GetIntegerField(TEXT("year"));
						int32 Month = ConcertTimeObject->GetIntegerField(TEXT("month"));
						int32 Day = ConcertTimeObject->GetIntegerField(TEXT("day"));
						FString Time = ConcertTimeObject->GetStringField(TEXT("time"));

						UE_LOG(LogTemp , Log , TEXT("Concert Time: %d-%d-%d %s") , Year , Month , Day , *Time);

						// TicketingUI에 Set Text // 콘서트 네임은 GI에서 가져올 수도 있음
						TicketingUI->SetConcertInfo(GetConcertName() , Year , Month , Day , Time);
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

						// TicketingUI에 Set Text
						TicketingUI->SetTickettingDate(Year , Month , Day);
						//TicketingUI->SetTextGameStartTime(Time);
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
void AHM_HttpActor2::ReqPostRegisterSeat(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("좌석 접수 조회 요청 SeatId : %s") , *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostRegisterSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 좌석 접수 요청에 대한 응답
void AHM_HttpActor2::OnResPostRegisterSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
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
					FString SeatId = ResponseObject->GetStringField(TEXT("seatId"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 RemainingTicket = ResponseObject->GetNumberField(TEXT("remainingTicket"));
					int32 CompetitionRate = ResponseObject->GetIntegerField(TEXT("competitionRate"));

					UE_LOG(LogTemp , Log , TEXT("SeatId : %s") , *SeatId);
					UE_LOG(LogTemp , Log , TEXT("seatPrice : %d") , SeatPrice);
					UE_LOG(LogTemp , Log , TEXT("RemainingTicket : %d") , RemainingTicket);
					UE_LOG(LogTemp , Log , TEXT("CompetitionRate : %d") , CompetitionRate);

					// DTO 생성 및 데이터 설정
					FMyReceptionSeatInfoDTO MySeatDTO;
					MySeatDTO.SetMySeatId(SeatId);
					UE_LOG(LogTemp , Log , TEXT("MySeatDTO.SetMySeatId(SeatId) : %s") , *MySeatDTO.GetMySeatId());
					SetMySeatId(SeatId);

					if( TTPlayer && GI && MainUI )
					{
						GI->SetRemainingTicketCount(RemainingTicket);
						MainUI->BuyTicketWidget->SetTextTicketPrice(SeatPrice);
					}

					if ( GI && TicketingUI )
					{
						// 접수 성공했을 때 UI연결
						SetSeatPrice(SeatPrice);
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
void AHM_HttpActor2::ReqPostMyRegisteredSeat(FString ConcertName , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/my/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostMyRegisteredSeat);

	// 요청 전송
	Request->ProcessRequest();
}

// 내가 접수한 좌석 조회 요청 응답
void AHM_HttpActor2::OnResPostMyRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					// "receptionSeatDTOList" 배열을 가져옵니다.
					TArray<TSharedPtr<FJsonValue>> ReceptionSeatArray = ResponseObject->GetArrayField(TEXT("receptionSeatDTOList"));

					// 배열 내의 각 객체를 처리합니다.
					for ( TSharedPtr<FJsonValue> SeatValue : ReceptionSeatArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();

						if ( SeatObject.IsValid() )
						{
							// seatInfo 추출 및 출력
							FString SeatInfo = SeatObject->GetStringField(TEXT("seatInfo"));
							UE_LOG(LogTemp , Log , TEXT("Seat Info: %s") , *SeatInfo);

							// concertTime 추출 및 출력
							TSharedPtr<FJsonObject> ConcertTimeObject = SeatObject->GetObjectField(TEXT("concertTime"));
							int32 ConcertYear = ConcertTimeObject->GetIntegerField(TEXT("year"));
							int32 ConcertMonth = ConcertTimeObject->GetIntegerField(TEXT("month"));
							int32 ConcertDay = ConcertTimeObject->GetIntegerField(TEXT("day"));
							FString ConcertTime = ConcertTimeObject->GetStringField(TEXT("time"));
							UE_LOG(LogTemp , Log , TEXT("Concert Time: %d-%d-%d %s") , ConcertYear , ConcertMonth , ConcertDay , *ConcertTime);

							// drawingTime 추출 및 출력
							TSharedPtr<FJsonObject> DrawingTimeObject = SeatObject->GetObjectField(TEXT("drawingTime"));
							int32 DrawingYear = DrawingTimeObject->GetIntegerField(TEXT("year"));
							int32 DrawingMonth = DrawingTimeObject->GetIntegerField(TEXT("month"));
							int32 DrawingDay = DrawingTimeObject->GetIntegerField(TEXT("day"));
							FString DrawingTime = DrawingTimeObject->GetStringField(TEXT("time"));
							UE_LOG(LogTemp , Log , TEXT("Drawing Time: %d-%d-%d %s") , DrawingYear , DrawingMonth , DrawingDay , *DrawingTime);

							// competitionRate 추출 및 출력
							int32 CompetitionRate = SeatObject->GetIntegerField(TEXT("competitionRate"));
							UE_LOG(LogTemp , Log , TEXT("Competition Rate: %d") , CompetitionRate);

						}
					}
					// 내가 접수한 좌석 확인 UI
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
void AHM_HttpActor2::ReqDeleteCancelRegisteredSeat(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/reception") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("DELETE"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("좌석 취소 조회 요청 SeatId : %s") , *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

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
					// 받아올 정보 추출
					int32 RemainingTicket = ResponseObject->GetIntegerField(TEXT("remainingTicket"));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("RemainingTicket : %d") , RemainingTicket));

					if ( TTPlayer && GI && MainUI )
					{
						GI->SetRemainingTicketCount(RemainingTicket);
					}

					if ( TicketingUI )
					{
						// 접수 취소 성공했을 때
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

// 추첨 시작 알림 요청
void AHM_HttpActor2::ReqPostNoticeGameStart(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/drawing") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("추첨 시작 알림 요청 SeatId : %s") , *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

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
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to get concert entry: %s") , *Response->GetContentAsString());
		}
	}
}

//=========================================================================================================================================

// 좌석 게임 결과
void AHM_HttpActor2::ReqPostGameResult(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/draw-result") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("좌석 게임 결과 요청 SeatId : %s") , *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	//Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResDeleteCancelRegisteredSeat);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResPostGameResult(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	//if ( bWasSuccessful && Response.IsValid() )
	//{
	//	UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
	//	UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

	//	if ( Response->GetResponseCode() == 200 )
	//	{
	//		UE_LOG(LogTemp , Log , TEXT("Request Post Game Result successful!"));
	//		// 추첨이 끝났을 때 당첨자에게만 예매창 UI 띄우는 함수에 연결
	//		if ( MainUI->GetBuyTicketWidget() )
	//		{
	//			MainUI->BuyTicketWidget->SetWidgetSwitcher(0);
	//		}
	//	}
	//	else if ( Response->GetResponseCode() == 400 )
	//	{
	//		UE_LOG(LogTemp , Log , TEXT("Request Post Game Result failed!"));
	//	}
	//}

	if ( bWasSuccessful )
	{
		UE_LOG(LogTemp , Log , TEXT("Request Post Game Result successful!"));
		// 추첨이 끝났을 때 당첨자에게만 예매창 UI 띄우는 함수에 연결
		if ( MainUI->GetBuyTicketWidget() )
		{
			MainUI->BuyTicketWidget->SetWidgetSwitcher(0);
		}
	}
	else
	{
		UE_LOG(LogTemp , Log , TEXT("Request Post Game Result failed!"));
	}
}

// 결제시 회원 인증용 QR 요청
void AHM_HttpActor2::ReqGetMemberAuthQR(FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/qr/verification") , *_url); // 미정
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
					FString UserCode = ResponseObject->GetStringField(TEXT("userCode"));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("UserCode : %s") , *UserCode));
					UE_LOG(LogTemp , Log , TEXT("UserCode : %s"), *UserCode);

					SetUserCode(UserCode);

					// Base64로 인코딩된 이미지 데이터 추출
					FString Base64Image = ResponseObject->GetStringField(TEXT("image"));
					TArray<uint8> ImageData;
					FBase64::Decode(Base64Image , ImageData);

					// 텍스처로 변환
					UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
					if ( Texture )
					{
						// 결제 시 회원 인증 QR UI로 넘어가는 함수 호출하기
						if ( MainUI->GetBuyTicketWidget() )
						{
							MainUI->BuyTicketWidget->SetQRImg(Texture);
							MainUI->SetWidgetSwitcher(3);

							UE_LOG(LogTemp , Log , TEXT("Image received and processed successfully."));
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
void AHM_HttpActor2::ReqGetPostConfirmMemberPhoto(FString UserCode , FString AccessToken)
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

	UE_LOG(LogTemp , Log , TEXT("회원인증 확인 요청 userCode : %s"), *UserCode);

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
			if ( MainUI->GetBuyTicketWidget() )
			{
				MainUI->BuyTicketWidget->SetTextSeatID(GetSeatFloor(), GetSeatInfo());
				UE_LOG(LogTemp , Log , TEXT("GetSeatFloor : %d / GetSeatInfo : %s"), GetSeatFloor() , *GetSeatInfo());
				MainUI->BuyTicketWidget->SetConcertInfo_BuyTicket(GetConcertName(),GetConcertYear(),GetConcertMonth(),GetConcertDay(),GetConcertTime());
				MainUI->BuyTicketWidget->SetWidgetSwitcher(1);
				UE_LOG(LogTemp , Log , TEXT("Member authentication was successful!"));
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

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/member/delivery-address") , *_url);
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

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if ( TTPlayer && GI )
					{
						GI->SetCoin(UserCoin);
					}

					SetSeatInfo(SeatInfo);
					SetSeatPrice(SeatPrice);
					
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
void AHM_HttpActor2::ReqPostPaymentSeat(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/reservation") , *_url); // API테이블 확인하기
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteValue(TEXT("seatId") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	UE_LOG(LogTemp , Log , TEXT("좌석 결제 요청 SeatId : %s") , *SeatId);

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

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
						GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
					}
					
					SetSeatId(SeatId);
					SetSeatInfo(SeatInfo);
					SetSeatPrice(SeatPrice);
					
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


// ======================== 개발자 키(2) Cheat Seat 좌석 게임 결과 요청 ========================
void AHM_HttpActor2::ReqPostCheatGameResult(FString ConcertName , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/draw-cheat") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 요청 전송
	Request->ProcessRequest();

	UE_LOG(LogTemp , Log , TEXT("Cheat Seat 좌석 게임 결과 요청"));
}

void AHM_HttpActor2::ReqPostCheatPaymentSeat(FString ConcertName, FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/reservation-cheat") , *_url); // API테이블 확인하기
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("concertName") , ConcertName);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

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
					
					 SetSeatInfo(SeatInfo);
					 //SetSeatPrice(SeatPrice);
					 //SetUserName(UserName);
					 //SetUserAddress1(UserAddress);
					if ( MainUI->GetBuyTicketWidget() )
					{
						//MainUI->BuyTicketWidget->SetTextSeatID2(SeatInfo);
						MainUI->BuyTicketWidget->SetTextTicketNum(SeatNum);
						MainUI->BuyTicketWidget->SetTextUserName(UserName);
						//MainUI->BuyTicketWidget->SetTextUserPhoneNum(UserPhoneNum);
						//MainUI->BuyTicketWidget->SetTextUserAddress(UserAddress);
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

