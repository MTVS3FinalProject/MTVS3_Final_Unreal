// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor.h"
#include "JMH/MH_StartWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HJ/TTGameInstance.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"

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

// QR코드 요청을 서버에 보내는 함수
void AHM_HttpActor::ReqPostGetVerifyIdentityQR(FText Email, FText Password)
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
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostGetVerifyIdentityQR);

	// 요청 실행
	Request->ProcessRequest();
}

// QR코드 요청에 대한 응답을 처리하는 함수
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
				//UE_LOG(LogTemp , Log , TEXT("Image Data Size: %d") , ImageData.Num());
				//UE_LOG(LogTemp , Log , TEXT("Texture created successfully: %s") , *Texture->GetName());
			}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("Failed to create texture from image data."));
			}
		}
		else if ( Response->GetResponseCode() == 400 ) // 실패 코드
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ErrorObject = JsonObject->GetObjectField(TEXT("error"));
				FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
				int32 ErrorStatus = ErrorObject->GetIntegerField(TEXT("status"));

				UE_LOG(LogTemp, Warning, TEXT("Error Message: %s"), *ErrorMessage);
				UE_LOG(LogTemp, Warning, TEXT("Error Status: %d"), ErrorStatus);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response."));
			}
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify IdentityQR, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

// 신원 확인 요청을 서버에 보내는 함수
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

// 신원 확인 요청에 대한 응답 처리하는 함수
void AHM_HttpActor::OnResPostVerifyIdentity(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공 코드
		{
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
					StartUI->SetWidgetSwitcher(4); // 회원가입 정보 입력 UI
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Identity Verification Failed"));
					// 실패 처리
				}
			}
		}
		else if ( Response->GetResponseCode() == 400 ) // 실패 코드
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ErrorObject = JsonObject->GetObjectField(TEXT("error"));
				FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
				int32 ErrorStatus = ErrorObject->GetIntegerField(TEXT("status"));

				UE_LOG(LogTemp, Warning, TEXT("Error Message: %s"), *ErrorMessage);
				UE_LOG(LogTemp, Warning, TEXT("Error Status: %d"), ErrorStatus);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response."));
			}
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify IdentityQR, response code: %d") , Response->GetResponseCode());
			//StartUI->(?); // 신원인증 확인 실패 UI 프론트에서 처리함
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

// 회원가입 요청을 서버에 보내는 함수
void AHM_HttpActor::ReqPostSignup(FText Nickname ,bool bIsHost , FText Email , FText Password , FString Birth , int32 AvataData)
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

	int32 isHost = bIsHost ? 1 : 0;
	
	// 전달 데이터 (JSON)
	FString ContentString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ContentString);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("nickname") , Nickname.ToString());
	Writer->WriteValue(TEXT("isHost") , isHost);
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteValue(TEXT("birth") , Birth);
	Writer->WriteValue(TEXT("avatarData") , AvataData);
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	UE_LOG(LogTemp , Log , TEXT("Content String: %s") , *ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostSignup);

	// 요청 실행
	Request->ProcessRequest();
}

// 회원가입 요청에 대한 응답을 처리하는 함수
void AHM_HttpActor::OnResPostSignup(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공 코드
		{
			// 성공 처리 (회원가입 완료)
			if ( StartUI )
			{
				StartUI->SetWidgetSwitcher(0);
			}
			UE_LOG(LogTemp , Log , TEXT("Sign-up success"));
		}
		else if ( Response->GetResponseCode() == 400 ) // 패스워드 유효성체크 실패 코드
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ErrorObject = JsonObject->GetObjectField(TEXT("error"));
				FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
				int32 ErrorStatus = ErrorObject->GetIntegerField(TEXT("status"));

				UE_LOG(LogTemp, Warning, TEXT("Error Message: %s"), *ErrorMessage);
				UE_LOG(LogTemp, Warning, TEXT("Error Status: %d"), ErrorStatus);

				// 에러메세지 반영해주는 UI SetText 함수 호출하기
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response."));
			}
			
			UE_LOG(LogTemp , Warning , TEXT("Failed to verify IdentityQR, response code: %d") , Response->GetResponseCode());
			//StartUI->OnLoginFail(?); // 이메일 중복. (로그인 실패 처리)
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

// 로그인 요청을 서버에 보내는 함수
void AHM_HttpActor::ReqPostLogin(FText Email , FText Password)
{
	// HTTP 모듈 가져오기
	FHttpModule* Http = &FHttpModule::Get();
	if ( !Http ) return;

	// HTTP 요청 생성
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	// 서버 URL 설정

	FString FormattedUrl = FString::Printf(TEXT("%s/auth/login") , *_url); // signin으로 바꿔야함?
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

// 로그인 요청에 대한 응답을 처리하는 함수
void AHM_HttpActor::OnResPostLogin(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		// 캐스팅은 여기서 한 번만 실행
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 200 ) // 성공적 응답 (코드 200)
		{
			// JSON 응답 파싱
			FString ResponseBody = Response->GetContentAsString();
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
			
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                // response 객체 접근
                TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField(TEXT("response"));
                if (ResponseObject.IsValid())
                {
                    // memberInfoDTO 객체를 JSON 문자열로 추출
                    TSharedPtr<FJsonObject> MemberInfo = ResponseObject->GetObjectField(TEXT("memberInfoDTO"));
                    if (MemberInfo.IsValid())
                    {
                        FString MemberInfoString;
                        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&MemberInfoString);
                        FJsonSerializer::Serialize(MemberInfo.ToSharedRef(), Writer);

                        // FPlayerData 구조체에 JSON 매핑
                        FPlayerData PlayerData;
                        if (FJsonObjectConverter::JsonObjectStringToUStruct(MemberInfoString, &PlayerData, 0, 0))
                        {
                            // authTokenDTO 데이터 수동 설정
                            TSharedPtr<FJsonObject> AuthTokenObject = ResponseObject->GetObjectField(TEXT("authTokenDTO"));
                            if (AuthTokenObject.IsValid())
                            {
                                PlayerData.accessToken = AuthTokenObject->GetStringField(TEXT("accessToken"));
                            }

                            // GameInstance에 PlayerData 설정
                            GI->SetPlayerData(PlayerData);
                            
                            // 로그 출력
                            UE_LOG(LogTemp, Log, TEXT("Nickname: %s"), *PlayerData.nickname);
                            UE_LOG(LogTemp, Log, TEXT("Coin: %d"), PlayerData.coin);
                            UE_LOG(LogTemp, Log, TEXT("AvatarData: %d"), PlayerData.avatarData);
                            UE_LOG(LogTemp, Log, TEXT("TitleName: %s"), *PlayerData.titleName);
                            UE_LOG(LogTemp, Log, TEXT("TitleRarity: %s"), *PlayerData.titleRarity);
                            UE_LOG(LogTemp, Log, TEXT("LuckyDrawSeatID: %s"), *PlayerData.LuckyDrawSeatID);
                            UE_LOG(LogTemp, Log, TEXT("AccessToken: %s"), *PlayerData.accessToken);

                            // 세션 입장
                            StartUI->GoToLobby();
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("FPlayerData 구조체로 JSON 응답 파싱 실패."));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("'memberInfoDTO' 객체를 찾을 수 없습니다."));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("'response' 객체를 찾을 수 없습니다."));
                }
            }
		}
		else if ( Response->GetResponseCode() == 400 ) // 실패 코드
		{
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				TSharedPtr<FJsonObject> ErrorObject = JsonObject->GetObjectField(TEXT("error"));
				FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
				int32 ErrorStatus = ErrorObject->GetIntegerField(TEXT("status"));

				UE_LOG(LogTemp, Warning, TEXT("Error Message: %s"), *ErrorMessage);
				UE_LOG(LogTemp, Warning, TEXT("Error Status: %d"), ErrorStatus);

				//StartUI->OnSigninFail(ErrorMessage); // 로그인 실패 메세지 Set Text
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON response."));
			}
			UE_LOG(LogTemp , Warning , TEXT("Failed to Sign in, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("요청 실패 또는 응답이 유효하지 않음"));
		//StartUI->OnSigninFail(); // 네트워크 오류 처리
	}
}