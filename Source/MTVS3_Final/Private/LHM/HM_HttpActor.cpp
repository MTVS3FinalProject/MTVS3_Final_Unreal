// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_HttpActor.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

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
	
	/*StartUI =  CastChecked<UStartWidget>(CreateWidget(GetWorld(), StartUIFactory));
	if ( StartUI )
	{
		StartUI->AddToViewport();
	}

	auto* pc = UGameplayStatics::GetPlayerController(this, 0);
	if( !pc ) return;
	pc->SetShowMouseCursor(true);
	pc->SetInputMode(FInputModeGameAndUI);*/
}

// Called every frame
void AHM_HttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_HttpActor::ReqPostVerifyIdentity(long UserId)
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
				bool bVerified = JsonObject->GetBoolField("verified");
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

void AHM_HttpActor::ReqPostOnVerifyIdentity(long UserId)
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
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostOnVerifyIdentity);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostOnVerifyIdentity(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
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

			if ( FJsonSerializer::Deserialize(Reader , JsonObject) && JsonObject.IsValid() )
			{
				// 필요한 데이터를 파싱하고 처리
				bool bVerified = JsonObject->GetBoolField("verified");
				if ( bVerified )
				{
					UE_LOG(LogTemp , Log , TEXT("Verification success"));
					// 성공 처리
				}
				else
				{
					UE_LOG(LogTemp , Warning , TEXT("Verification failed"));
					// 실패 처리
				}
			}
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to get verification, response code: %d") , Response->GetResponseCode());
		}
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("Request failed or invalid response"));
	}
}

void AHM_HttpActor::ReqPostSignin(bool bIsHost , FText Email , FText Password , FText Age , FString Gender , FText Nickname)
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
	Writer->WriteValue(TEXT("isHost"), bIsHost);
	Writer->WriteValue(TEXT("email") , Email.ToString());
	Writer->WriteValue(TEXT("password") , Password.ToString());
	Writer->WriteValue(TEXT("age") , Age.ToString());
	Writer->WriteValue(TEXT("gender") , Gender);
	Writer->WriteValue(TEXT("nickname") , Nickname.ToString());
	Writer->WriteObjectEnd();
	Writer->Close();

	// 요청 본문에 JSON 데이터를 설정
	Request->SetContentAsString(ContentString);

	// 응답받을 함수를 연결
	Request->OnProcessRequestComplete().BindUObject(this , &AHM_HttpActor::OnResPostSignin);

	// 요청 실행
	Request->ProcessRequest();
}

void AHM_HttpActor::OnResPostSignin(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	if ( bWasSuccessful && Response.IsValid() )
	{
		UE_LOG(LogTemp , Log , TEXT("Response Code: %d") , Response->GetResponseCode());
		UE_LOG(LogTemp , Log , TEXT("Response Body: %s") , *Response->GetContentAsString());

		if ( Response->GetResponseCode() == 201 ) // 회원가입 성공 응답 코드 (201 Created)
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
		// 요청 실패 또는 응답이 유효하지 않은 경우 처리
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
	Requset->SetURL(TEXT(""));
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
				TSharedPtr<FJsonObject> ResponseObject = JsonObject->GetObjectField("response");

				if ( ResponseObject.IsValid() )
				{
					// "accessToken"과 "nickname" 추출
					FString AccessToken = ResponseObject->GetStringField("accessToken");
					FString Nickname = ResponseObject->GetStringField("nickname");

					//GameInstance* GI = GetWorld()->GetGameInstance<GameInstance>();
					//if ( !GI ) return;
					//GI->SetPlayerNickname(Nickname);

					if ( !AccessToken.IsEmpty() )
					{
						UE_LOG(LogTemp , Log , TEXT("Received Access Token: %s") , *AccessToken);
						UE_LOG(LogTemp , Log , TEXT("Received Nickname: %s") , *Nickname);

						// 로그인 성공 시 처리
						// GameInstance에 토큰(고유ID) 및 닉네임 저장
						//GI = GetWorld()->GetGameInstance<GameInstance>();
						//if ( !GI ) return;
						//GI->SetAccessToken(AccessToken);
						//StartUI->OnLoginSuccess();
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("Access Token is missing in the response."));
						//StartUI->OnLoginFail(1); // 로그인 실패 처리
					}
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

