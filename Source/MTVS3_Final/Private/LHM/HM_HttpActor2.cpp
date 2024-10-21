// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor2.h"
#include "JMH/MH_TicketingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "LHM/TTPlayerController.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "ImageUtils.h"

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
	
	//if ( UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("TTHallMap") )
	//{
		// 특정 레벨일 때 실행할 코드
		//UE_LOG(LogTemp , Warning , TEXT("현재 레벨은 TTHallMap입니다."));

		//TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));

		//// 테스트용
		//if ( TicketingUI )
		//{
		//	TicketingUI->AddToViewport();
		//}

		//auto* pc = UGameplayStatics::GetPlayerController(this , 0);
		//if ( !pc ) return;
		//pc->SetShowMouseCursor(true);
		//pc->SetInputMode(FInputModeGameAndUI());
	//}
}

// Called every frame
void AHM_HttpActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_HttpActor2::SetTicketingUI(UMH_TicketingWidget* InTicketingUI)
{
	TicketingUI = InTicketingUI; // 전달받은 TicketingUI 참조 저장
}

//=========================================================================================================================================

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
					int32 Year = ResponseObject->GetIntegerField(TEXT("year"));
					int32 Month = ResponseObject->GetIntegerField(TEXT("month"));
					int32 Day = ResponseObject->GetIntegerField(TEXT("day"));
					FString Time = ResponseObject->GetStringField(TEXT("time"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("ConcertName : %s") , *ConcertName));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("remainingTickets : %d"), RemainingTickets));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Year : %d"), Year));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Month : %d") , Month));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Day : %d") , Day));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Time : %s") , *Time));

					// RemainingTickets을 GameInstance에 저장
					GI->SetRemainingTicketCount(RemainingTickets);
					// 콘서트 정보 저장
					SetConcertInfo(ConcertName , Year , Month , Day , Time);
					
					// 접수 가능한 좌석 목록
					TArray<TSharedPtr<FJsonValue>> AvailableSeatsArray = ResponseObject->GetArrayField(TEXT("availableSeats"));
					TArray<FSeatIdDTO> AvailableSeats;

					for ( const TSharedPtr<FJsonValue>& SeatValue : AvailableSeatsArray )
					{
						TSharedPtr<FJsonObject> SeatObject = SeatValue->AsObject();
						FString AvailableSeatId = SeatObject->GetStringField(TEXT("seatId"));

						UE_LOG(LogTemp , Log , TEXT("Available Seat ID: %s") , *AvailableSeatId);
						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(AvailableSeatId);
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

						UE_LOG(LogTemp , Log , TEXT("Reception Seat ID: %s") , *ReceptionSeatId);
						FSeatIdDTO SeatDTO;
						SeatDTO.SetSeatId(ReceptionSeatId);
						ReceptionSeats.Add(SeatDTO);
					}
					// 콘서트 입장하는 함수 호출
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("콘서트 입장~~~")));
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
					FString SeatId = JsonObject->GetStringField(TEXT("seatId")); // 로컬 좌석의 공연날짜+넘버와 서버의 seatID가 같은지만 확인
					FString SeatInfo = JsonObject->GetStringField(TEXT("seatInfo")); // 로컬 좌석 넘버와 서버의 seatInfo가 같은지만 확인
					//FString DrawingTime = JsonObject->GetStringField(TEXT("drawingTime"));
					int32 CompetitionRate = JsonObject->GetIntegerField(TEXT("competitionRate"));

					// 로그로 출력
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Seat ID: %s") , *SeatId));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Seat Info: %s") , *SeatInfo));
					//GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Drawing Time: %s") , *DrawingTime));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Competition Rate: %d") , CompetitionRate));

					// Ticketing UI에 당첨 확률 SetText
					if ( TicketingUI )
					{
						//TicketingUI->SetTextCompetitionRate(CompetitionRate);
						//TicketingUI->SetVisibleSwitcher(true);
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
					int32 RemainingTicket = ResponseObject->GetNumberField(TEXT("remainingTicket"));
					int32 CompetitionRate = JsonObject->GetIntegerField(TEXT("competitionRate"));
					int32 SeatPrice = JsonObject->GetIntegerField(TEXT("seatPrice"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("RemainingTicket : %d") , RemainingTicket));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Competition Rate : %d") , CompetitionRate));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("seatPrice : %d") , SeatPrice));

					if( TTPlayer && GI )
					{
						GI->UseRemainingTicket(1);
					}

					if ( TicketingUI )
					{
						// 접수 성공했을 때 UI연결
						SetSeatPrice(SeatPrice);
						//TicketingUI->SetTextTicketPrice(SeatPrice);
						//TicketingUI->SetTextCompetitionRate(CompetitionRate);
						//TicketingUI->SetCompletedVisible(true);
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

void AHM_HttpActor2::OnResDeleteCancelRegisteredSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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
					int32 RemainingTicket = ResponseObject->GetIntegerField(TEXT("remainingTicket"));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("RemainingTicket : %d") , RemainingTicket));

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					if ( TTPlayer )
					{
						UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
						if ( GI )
						{
							GI->UseRemainingTicket(-1);
						}
					}

					if ( TicketingUI )
					{
						// 접수 취소 성공했을 때
						//TicketingUI->다시 접수 가능하게 바뀌는 함수 ( 아 직 안 함 )
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

//=========================================================================================================================================

void AHM_HttpActor2::ReqPostGameResult(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/게임결과") , *_url); // 미정
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
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResDeleteCancelRegisteredSeat);

	// 요청 전송
	Request->ProcessRequest();
}

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

void AHM_HttpActor2::OnResGetMemberAuthQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
		{
			//TArray<uint8> ImageData = Response->GetContent();
			//FString imagePath = FPaths::ProjectPersistentDownloadDir();
			//FFileHelper::SaveArrayToFile(ImageData , *imagePath);
			//UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
			//if ( Texture )
			//{
			//	// 결제 시 회원 인증 QR UI로 넘어가는 함수 호출하기
			//	// UI-> ????
			//	UE_LOG(LogTemp , Log , TEXT("Image received and processed successfully."));
			//}
			//else
			//{
			//	UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
			//}

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
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("userCode : %s") , *UserCode));

					SetUserCode(UserCode);

					// Base64로 인코딩된 이미지 데이터 추출
					FString Base64Image = ResponseObject->GetStringField(TEXT("image"));
					TArray<uint8> ImageData;
					FBase64::Decode(Base64Image , ImageData);

					// 이미지 데이터를 파일로 저장 (선택적)
					//FString imagePath = FPaths::ProjectPersistentDownloadDir() / TEXT("AuthQR.png");
					//FFileHelper::SaveArrayToFile(ImageData , *imagePath);

					// 텍스처로 변환
					UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
					if ( Texture )
					{
						// 결제 시 회원 인증 QR UI로 넘어가는 함수 호출하기
						// UI-> ????
						UE_LOG(LogTemp , Log , TEXT("Image received and processed successfully."));
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

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResGetPostConfirmMemberPhoto);

	// 요청 전송
	Request->ProcessRequest();
}

void AHM_HttpActor2::OnResGetPostConfirmMemberPhoto(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 )
		{
			UE_LOG(LogTemp , Log , TEXT("Member authentication was successful!"));
		}
		else if ( Response->GetResponseCode() == 400 )
		{
			UE_LOG(LogTemp , Log , TEXT("Member authentication failed!"));
		}
	}
}

void AHM_HttpActor2::ReqPostReservationinfo(FString UserName , int32 UserPhoneNum , FString UserAddress , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/payment") , *_url); // API테이블 확인하기
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));

	// 헤더 설정
	Request->SetHeader(TEXT("Authorization") , FString::Printf(TEXT("Bearer %s") , *AccessToken));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userName") , UserName);
	Writer->WriteValue(TEXT("userPhoneNumber") , UserPhoneNum);
	Writer->WriteValue(TEXT("userAddress") , UserAddress);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor2::OnResPostReservationinfo);

	// 요청 전송
	Request->ProcessRequest();
}

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
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 Coin = ResponseObject->GetIntegerField(TEXT("Coin"));
					int32 NeedCoin = ResponseObject->GetIntegerField(TEXT("needCoin"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("SeatInfo : %s") , *SeatInfo));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("SeatPrice : %d") , SeatPrice));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Coin : %d") , Coin));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("NeedCoin : %d") , NeedCoin));
					
					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if ( TTPlayer && GI )
					{
						GI->SetCoin(Coin);
					}

					SetSeatInfo(SeatInfo);
					SetSeatPrice(SeatPrice);
					SetNeedCoin(NeedCoin);

					// 결제 완료 UI 호출
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Reservation info: %s") , *Response->GetContentAsString());
		}
	}
}
 
void AHM_HttpActor2::ReqPostPaymentSeat(FString ConcertName , FString SeatId , FString AccessToken)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/concert/seat/payment") , *_url); // API테이블 확인하기
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
					FString SeatInfo = ResponseObject->GetStringField(TEXT("seatInfo"));
					int32 SeatPrice = ResponseObject->GetIntegerField(TEXT("seatPrice"));
					int32 Coin = ResponseObject->GetIntegerField(TEXT("Coin"));
					FString UserName = ResponseObject->GetStringField(TEXT("userName"));
					FString UserAddress = ResponseObject->GetStringField(TEXT("userAddress"));

					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("SeatInfo : %s") , *SeatInfo));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("SeatPrice : %d") , SeatPrice));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("UserCoin : %d") , Coin));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("UserName : %s") , *UserName));
					GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("UserAddress : %s") , *UserAddress));

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
					if ( TTPlayer && GI )
					{
						GI->SetCoin(Coin);
					}
					
					SetSeatInfo(SeatInfo);
					SetSeatPrice(SeatPrice);
					SetUserName(UserName);
					SetUserAddress(UserAddress);

					// 결제 완료 UI 호출
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Failed to Post Payment Seat: %s") , *Response->GetContentAsString());
		}
	}
}
