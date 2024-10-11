// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor.h"
#include "JMH/MH_StartWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include <chrono>
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"
#include "ImageUtils.h"

// Sets default values
AHM_HttpActor::AHM_HttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHM_HttpActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartUI =  CastChecked<UMH_StartWidget>(CreateWidget(GetWorld(), StartUIFactory));
	if ( StartUI )
	{
		StartUI->AddToViewport();
	}

	auto* pc = UGameplayStatics::GetPlayerController(this, 0);
	if( !pc ) return;
	pc->SetShowMouseCursor(true);
	pc->SetInputMode(FInputModeUIOnly());
}

// Called every frame
void AHM_HttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_HttpActor::ReqPostGetVerifyIdentityQR(FText Email)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정
	Request->SetURL(TEXT("/api/qr/verification"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostGetVerifyIdentityQR);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostGetVerifyIdentityQR(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
		{
			//// 응답 처리
			//FString ResponseBody = Response->GetContentAsString();
			//TSharedPtr<FJsonObject> JsonObject;
			//TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			//if ( FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid() )
			//{
			//	// 필요한 데이터를 파싱하고 처리
			//	bool bVerified = JsonObject->GetBoolField(TEXT("verified"));
			//	if ( bVerified )
			//	{
			//		UE_LOG(LogTemp , Log , TEXT("IdentityQR Verified Successfully"));
			//		// QR코드 이미지 요청 성공
			//	}
			//	else
			//	{
			//		UE_LOG(LogTemp , Warning , TEXT("IdentityQR Verification Failed"));
			//		// 실패 처리
			//	}
			//}
			
			// 이미지 데이터 처리
			// 이미지 데이터로 텍스처 생성
			// 텍스처 생성 후 적용
			TArray<uint8> ImageData = Response->GetContent();
			FString imagePath = FPaths::ProjectPersistentDownloadDir();
			FFileHelper::SaveArrayToFile(ImageData , *imagePath);
			UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
			if ( Texture )
			{
				//StartUI->SetWebImage(realTexture);
				UE_LOG(LogTemp , Log , TEXT("Image received and processed successfully."));
			}
			else
			{
				
				UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
			}
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify IdentityQR, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostVerifyIdentity(int32 UserId)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정
	Request->SetURL(TEXT(""));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userId") , UserId); // UserId 전달
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostVerifyIdentity);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostVerifyIdentity(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
		{
			// 응답 처리
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			if ( FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid() )
			{
				// 필요한 데이터를 파싱하고 처리
				bool bVerified = JsonObject->GetBoolField(TEXT("verified"));
				if ( bVerified )
				{
					UE_LOG(LogTemp, Log, TEXT("Identity Verified Successfully"));
					// 로그인 또는 다음 단계로 넘어가는 처리
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Identity Verification Failed"));
					// 실패 처리
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify identity, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostSignup(bool bIsHost , FText Email , FText Password , int32 Age , FText Nickname , int32 AvataData)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정
	Request->SetURL(TEXT(""));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("isHost") , bIsHost);
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteValue(TEXT("age") , Age);
	Writer->WriteValue(TEXT("display_name") , Nickname.ToString());
	Writer->WriteValue(TEXT("avatarData") , AvataData);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostSignup);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostSignup(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공 응답 코드 (200)
		{
			// 성공 처리 (회원가입 완료)
			UE_LOG(LogTemp , Log , TEXT("Sign-up success"));
		}
		else
		{
			// 실패 처리
			UE_LOG(LogTemp , Warning , TEXT("Sign-up failed, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostLogin(FText Email , FText Password)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Requset = Http->CreateRequest();

	// 서버 URL 설정
	Requset->SetURL(TEXT("/api/auth/login"));
	Requset->SetVerb(TEXT("POST"));
	Requset->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Requset->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Requset->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostLogin);

	// 요청 실행
	Requset->ProcessRequest();
}

void AHM_HttpActor::OnResPostLogin(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		// 응답 본문을 로그에 출력
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
					FString Nickname = ResponseObject->GetStringField(TEXT("display_name"));
					int32 UserId = ResponseObject->GetIntegerField(TEXT("user_id"));
					int32 Age = ResponseObject->GetIntegerField(TEXT("age"));
					int32 Coin = ResponseObject->GetIntegerField(TEXT("coin"));
					//bool bIsHost = ResponseObject->GetIntegerField(TEXT("isHost"));
					int32 RemainingTicketCount = ResponseObject->GetIntegerField(TEXT("remainingTicketCount"));
					int32 AvatarData = ResponseObject->GetIntegerField(TEXT("avatarData"));

					ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
					if ( TTPlayer )
					{
						UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
						if ( GI )
						{
							// 닉네임 설정 및 가져오기
							GI->SetNickname(Nickname);
							UE_LOG(LogTemp , Log , TEXT("Nickname: %s") , *GI->GetNickname());

							// 서버에서 주는 UserId 설정 및 가져오기
							// 로그인 시 HTTP 통신으로 응답을 받아와 저장하는 방식
							GI->SetUserId(UserId);
							UE_LOG(LogTemp , Log , TEXT("UserId: %d") , GI->GetUserId());

							// 나이 설정 및 가져오기
							GI->SetAge(Age);
							UE_LOG(LogTemp , Log , TEXT("Age : %d") , GI->GetAge());

							// 코인 더하기 및 가져오기
							GI->SetCoin(Coin);
							UE_LOG(LogTemp , Log , TEXT("Coin: %d") , GI->GetCoin());

							// 아바타 설정 및 가져오기
							GI->SetAvatarData(AvatarData);
							UE_LOG(LogTemp , Log , TEXT("AvatarData : %d") , GI->GetAvatarData());

							// 티켓 접수 및 접수 가능 개수 가져오기
							// UseRemainingTicket의 매개변수는 티켓 접수 개수
							GI->SetRemainingTicketCount(RemainingTicketCount);
							UE_LOG(LogTemp , Log , TEXT("Remaining Tickets: %d") , GI->GetRemainingTicketCount());
						}
					}

					//if ( !AccessToken.IsEmpty() )
					//{
					//	//UE_LOG(LogTemp , Log , TEXT("Received Access Token: %s") , *AccessToken);
					//	UE_LOG(LogTemp , Log , TEXT("Received Nickname: %s") , *Nickname);

					//	// 로그인 성공 시 처리
					//	// GameInstance에 토큰(고유ID) 및 닉네임 저장
					//	//GI = GetWorld()->GetGameInstance<GameInstance>();
					//	//if ( !GI ) return;
					//	//GI->SetAccessToken(AccessToken);
					//	//StartUI->OnLoginSuccess();
					//}
					//else
					//{
					//	UE_LOG(LogTemp , Warning , TEXT("Access Token is missing in the response."));
					//	//StartUI->OnLoginFail(1); // 로그인 실패 처리
					//}
				}
				else
				{
					UE_LOG(LogTemp , Warning , TEXT("'response' 객체를 찾을 수 없습니다."));
					//StartUI->OnLoginFail(1); // 로그인 실패 처리
				}
			}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("JSON 응답 파싱 실패."));
				//StartUI->OnLoginFail(1); // 로그인 실패 처리
			}
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("로그인 실패, 응답 코드: %d") , Response->GetResponseCode());
			//StartUI->OnLoginFail(1); // 로그인 실패 처리
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("요청 실패 또는 응답이 유효하지 않음"));
		//StartUI->OnLoginFail(2); // 네트워크 오류 처리
	}
}

void AHM_HttpActor::ReqPostJoinTTSession(int32 UserId , int64 TTSessionId)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Requset = Http->CreateRequest();

	// 서버 URL 설정
	Requset->SetURL(TEXT(""));
	Requset->SetVerb(TEXT("POST"));
	Requset->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userID"), UserId);
	Writer->WriteValue(TEXT("ttSessionID"), TTSessionId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Requset->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Requset->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostJoinTTSession);

	// 요청 실행
	Requset->ProcessRequest();
}

void AHM_HttpActor::OnResPostJoinTTSession(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적인 응답 코드 200
		{
			// 응답 본문 처리 (필요한 정보가 있을 경우)
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			if ( FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid() )
			{
				// 서버 응답에서 필요한 데이터 추출
				bool bJoinSuccess = JsonObject->GetBoolField(TEXT("joinSuccess"));
				if ( bJoinSuccess )
				{
					UE_LOG(LogTemp , Log , TEXT("Successfully joined TT session."));
					// TT 세션 입장 성공 처리
				}
				else
				{
					UE_LOG(LogTemp , Warning , TEXT("Failed to join TT session."));
					// TT 세션 입장 실패 처리
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to join TT session, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostApplyForSeat(int32 UserId , int64 SeatId)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Requset = Http->CreateRequest();

	// 서버 URL 설정
	Requset->SetURL(TEXT(""));
	Requset->SetVerb(TEXT("POST"));
	Requset->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 요청 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("userID") , UserId);
	Writer->WriteValue(TEXT("seatID") , SeatId);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Requset->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Requset->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostApplyForSeat);

	// 요청 실행
	Requset->ProcessRequest();
}

void AHM_HttpActor::OnResPostApplyForSeat(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적인 응답 코드 200
		{
			// 응답 본문 처리 (필요한 정보가 있을 경우)
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

			if ( FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid() )
			{
				// 서버 응답에서 필요한 데이터 추출
				bool bSuccess = JsonObject->GetBoolField(TEXT("success"));
				if ( bSuccess )
				{
					UE_LOG(LogTemp , Log , TEXT("Successfully applied for seat"));
					// 좌석 신청 요청 성공
				}
				else
				{
					UE_LOG(LogTemp , Warning , TEXT("Failed to apply for seat"));
					// 좌석 신청 요청 실패
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostVerifyBooking(int32 UserId , int64 SeatId)
{

}

void AHM_HttpActor::OnResPostVerifyBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{

}

void AHM_HttpActor::ReqPostOnVerifyBooking(int32 UserId , int64 SeatId)
{

}

void AHM_HttpActor::OnResPostOnVerifyBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{

}

void AHM_HttpActor::ReqPostCompleteBooking(int32 UserId , int64 SeatId)
{

}

void AHM_HttpActor::OnResPostCompleteBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{

}

void AHM_HttpActor::ReqPostCancleBooking(int32 UserId , int64 SeatId)
{

}

void AHM_HttpActor::OnResPostCancleBooking(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{

}

