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
#include "Unix/UnixPlatformHttp.h"

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
	
	if ( UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("TTLobbyMap") )
	{
		// 특정 레벨일 때 실행할 코드
		UE_LOG(LogTemp , Warning , TEXT("현재 레벨은 TTLobbyMap입니다."));
		
		StartUI = CastChecked<UMH_StartWidget>(CreateWidget(GetWorld() , StartUIFactory));
		if ( StartUI )
		{
			StartUI->AddToViewport();
		}

		auto* pc = UGameplayStatics::GetPlayerController(this , 0);
		if ( !pc ) return;
		pc->SetShowMouseCursor(true);
		pc->SetInputMode(FInputModeUIOnly());
	}
}

// Called every frame
void AHM_HttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//=========================================================================================================================================

void AHM_HttpActor::ReqPostGetVerifyIdentityQR(FText Email)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	FString FormattedUrl = FString::Printf(TEXT("%s/qr/signup") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
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
			TArray<uint8> ImageData = Response->GetContent();
			FString imagePath = FPaths::ProjectPersistentDownloadDir();
			FFileHelper::SaveArrayToFile(ImageData , *imagePath);
			UTexture2D* Texture = FImageUtils::ImportBufferAsTexture2D(ImageData);
			if ( Texture )
			{
				// StartUI에서 QR UI로 넘어가는 함수 호출하기
				StartUI->SetQRImg(Texture);
				StartUI->SetWidgetSwitcher(3);
				UE_LOG(LogTemp , Log , TEXT("Image Data Size: %d") , ImageData.Num());
				UE_LOG(LogTemp , Log , TEXT("Texture created successfully: %s") , *Texture->GetName());
			}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
			}
		}
		else if ( Response->GetResponseCode() == 400 ) // 이메일 중복 코드
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify IdentityQR, response code: %d") , Response->GetResponseCode());
			//StartUI->OnLoginFail(?); // 이메일 중복. (로그인 실패 처리)
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
		//StartUI->OnLoginFail(?); // 그 외 네트워크 연결 확인. (회원가입 실패 처리)
	}
}

void AHM_HttpActor::ReqPostVerifyIdentity(FText Email)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정
	FString FormattedUrl = FString::Printf(TEXT("%s/qr/signup/success") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("email") , Email.ToString());
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
				bool bVerified = JsonObject->GetBoolField(TEXT("success"));
				if ( bVerified )
				{
					UE_LOG(LogTemp, Log, TEXT("Identity Verified Successfully"));
					// 로그인 또는 다음 단계로 넘어가는 처리
					StartUI->SetWidgetSwitcher(4);
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

void AHM_HttpActor::ReqPostSignup(bool bIsHost , FText Email , FText Password , FString Birth , FText Nickname , int32 AvataData)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정
	FString FormattedUrl = FString::Printf(TEXT("%s/auth/signup") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	//Writer->WriteValue(TEXT("isHost") , bIsHost);
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteValue(TEXT("birth") , Birth);
	Writer->WriteValue(TEXT("nickname") , Nickname.ToString());
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
			if ( StartUI )
			{
				StartUI->SetWidgetSwitcher(0);
			}
			UE_LOG(LogTemp , Log , TEXT("Sign-up success"));
		}
		else if(Response->GetResponseCode() == 400) // 닉네임 중복 응답 코드 (400)
		{
			// 실패 처리 ( 닉네임 중복 )
			//StartUI->OnLoginFail(?); // 닉네임 중복. (회원가입 실패 처리)
			UE_LOG(LogTemp , Warning , TEXT("Sign-up failed, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

//=========================================================================================================================================

void AHM_HttpActor::ReqPostLogin(FText Email , FText Password)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정

	FString FormattedUrl = FString::Printf(TEXT("%s/auth/login") , *_url);
	Request->SetURL(FormattedUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type") , TEXT("application/json"));

	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostLogin);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostLogin(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

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
					// "memberInfoDTO" 객체에 접근
					TSharedPtr<FJsonObject> MemberInfo = ResponseObject->GetObjectField(TEXT("memberInfoDTO"));
					if ( MemberInfo.IsValid() )
					{
						// 받아올 정보 추출
						FString Nickname = MemberInfo->GetStringField(TEXT("nickname"));
						FString Birth = MemberInfo->GetStringField(TEXT("birth"));
						int32 Coin = MemberInfo->GetIntegerField(TEXT("coin"));
						//int32 AvatarData = MemberInfo->GetIntegerField(TEXT("avatarData"));

						// 디버그 메시지 출력
						GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Nickname: %s") , *Nickname));
						GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Birth: %s") , *Birth));
						GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Coin: %d") , Coin));
						//GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("AvatarData: %d") , AvatarData));

						if ( TTPlayer )
						{
							if ( GI )
							{
								// 닉네임 설정 및 가져오기
								GI->SetNickname(Nickname);
								UE_LOG(LogTemp , Log , TEXT("Nickname: %s") , *GI->GetNickname());

								// 나이 설정 및 가져오기
								GI->SetBirth(Birth);
								const char* CStr = TCHAR_TO_ANSI(*GI->GetBirth());
								UE_LOG(LogTemp , Log , TEXT("Birth : %hs") , CStr);

								// 코인 더하기 및 가져오기
								GI->SetCoin(Coin);
								UE_LOG(LogTemp , Log , TEXT("Coin: %d") , GI->GetCoin());

								// 아바타 설정 및 가져오기
								//GI->SetAvatarData(AvatarData);
								//UE_LOG(LogTemp , Log , TEXT("Coin: %d") , GI->GetAvatarData());
							}
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("'memberInfoDTO' 객체를 찾을 수 없습니다."));
					}
					// "authTokenDTO" 객체에 접근
					TSharedPtr<FJsonObject> AuthTokenObject = ResponseObject->GetObjectField(TEXT("authTokenDTO"));
					if ( AuthTokenObject.IsValid() )
					{
						FString AccessToken = AuthTokenObject->GetStringField(TEXT("accessToken"));
						UE_LOG(LogTemp , Log , TEXT("AccessToken: %s") , *AccessToken);

						if ( TTPlayer )
						{
							if ( GI )
							{
								// 토큰 설정 및 가져오기
								GI->SetAccessToken(AccessToken);
								UE_LOG(LogTemp , Log , TEXT("AccessToken: %s") , *GI->GetAccessToken());
							}
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("'authTokenDTO' 객체를 찾을 수 없습니다."));
					}
					// 세션 입장
					StartUI->GoToLobby();
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
		else if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
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

//=========================================================================================================================================
