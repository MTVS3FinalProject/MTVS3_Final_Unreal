﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_StartWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor.h"

void UMH_StartWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//Login
	//이부분 확인해보기, 로그인 함수 하나 더 만들어서 그안에서 세션 생성해야함
	Btn_GoToLobby_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSignInButton);
	Btn_SignUp_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSignUpButton);
	Btn_test_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::Test_CreateSesstion); //테스트 세션생성 버튼
	Btn_ForgotPassword_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedForgotPasswordButton);
	// 종료
	Btn_Exit_LoginWin->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedExitWinButton);
	//Btn_Exit_LoginWin->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedForgotPasswordButton);
	Btn_ExitLogin->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedExitButton);
	Btn_BackLogin->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBackButton);


	//Signup
	Btn_Confirm_Signup->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirmSignupButton);
	Btn_Back_Signup->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBackButton);
	Btn_FAN->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedFANButton);
	Btn_MANAGER->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedMANAGERButton);
	Btn_SelectAvatarL->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarLButton);
	Btn_SelectAvatarR->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarRButton);
	//Avatar
	Btn_Confirm_Avatar->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedAvatarConfirmButton);

	//QR1
	Btn_Confirm_QRUi1->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirm_QRUi1Button);
	Btn_Back_QRUi1->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBack_QRUi1Button);
	//QR2
	Btn_Confirm_QRUi2->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirm_QRUi2Button);

	//LoginError 델리게이트에 바인딩
	if (WBP_MH_ErrorMessage01)
	{
		WBP_MH_ErrorMessage01->Btn_ErrorExit->OnClicked.AddDynamic(this , &UMH_StartWidget::HideLoginErrorMessage);
	}

	// OnTextChanged 델리게이트에 함수 바인딩
	EText_SignupBirth->OnTextChanged.AddDynamic(this , &UMH_StartWidget::OnTextChanged);

	// KHJ
	Img_Loading->SetVisibility(ESlateVisibility::Hidden);


	// // 게임 인스턴스를 가져와서
	// auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	// if (gi)
	// {
	// 	gi->OnFindSignatureCompleteDelegate.AddDynamic(this , &UMH_StartWidget::SetLoadingActive); // 세션 탐색 또는 생성
	// }
}

void UMH_StartWidget::ShowLoginErrorMessage(FString ErrorMS)
{
	WBP_MH_ErrorMessage01->ShowErrorMessage(ErrorMS);
	WBP_MH_ErrorMessage01->SetVisibility(ESlateVisibility::Visible);
}

void UMH_StartWidget::HideLoginErrorMessage()
{
	WBP_MH_ErrorMessage01->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_StartWidget::Test_CreateSesstion()
{
	GoToLobby();
}

void UMH_StartWidget::GoToLobby()
{
	//로비맵으로 이동(세션 생성,입장)
	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Create Lobby Session"));

	// KHJ
	// 게임 인스턴스를 가져와서
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->FindOrCreateSession(TEXT("TTHallSession") , 100); // 세션 탐색 또는 생성
	}

	SetLoadingActive(true);
}

//로그인 버튼
void UMH_StartWidget::OnClickedSignInButton()
{
	//유저정보 확인
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		HttpActor->ReqPostLogin(EText_Email->GetText() , EText_PassWord->GetText());
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Login Error"));
	}
	//만약 성공하면 로비로 이동
	//GoToLobby();
}

//회원가입하러 
void UMH_StartWidget::OnClickedSignUpButton()
{
	//회원가입 스위쳐 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMH_StartWidget::OnClickedExitWinButton()
{
	//게임 종료 창 띄우기
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(5);
}

void UMH_StartWidget::OnClickedExitButton()
{
	//게임 종료
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;

	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(World , PlayerController , EQuitPreference::Quit , true);
	}
}


void UMH_StartWidget::OnClickedForgotPasswordButton()
{
	//비번찾기
	ShowLoginErrorMessage(TEXT("서비스 준비중입니다."));
}

//QR 확인 
void UMH_StartWidget::OnClickedConfirm_QRUi1Button()
{
	//확인 버튼 누르면(얼굴인식 확인)->
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		HttpActor->ReqPostVerifyIdentity(EText_SignupEmail->GetText());
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("ClickedSignUp Error"));
	}

	//확인완료 ui로->
	//WS_StartWidgetSwitcher->SetActiveWidgetIndex(4);
}

void UMH_StartWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMH_StartWidget::OnClickedBack_QRUi1Button()
{
	//회원가입으로 다시 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(1);
}

//Http에서 서버로 QR 요청
void UMH_StartWidget::SetQRImg(UTexture2D* newTexture)
{
	//QR 받아오기
	Img_QR->SetBrushFromTexture(newTexture);
}

void UMH_StartWidget::OnClickedConfirm_QRUi2Button()
{
	//확인버튼 누르면 ->
	//아바타 설정ui로 이동.
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(2);
	PlayerImgOffAnim(1);
}

void UMH_StartWidget::OnClickedConfirmSignupButton()
{
	//회원가입 완료 -> 유저정보 서버로 전달
	//비번 중복확인.
	FText Password1 = EText_SignupPassWord->GetText();
	FText Password2 = EText_SignupPassWord2->GetText();

	//모두 입력했는지 확인
	//아바타 설정으로 이동

	//비밀번호 같은지 확인
	if (Password1.EqualTo(Password2))
	{
		//QR을 서버가 전달 성공했다면
		AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
		if (HttpActor)
		{
			HttpActor->ReqPostGetVerifyIdentityQR(EText_SignupEmail->GetText() , EText_SignupPassWord->GetText());
		}
		else
		{
			//에러창
			//GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("ClickedSignUp Error"));
		}

		//QR띄워주는 ui로 이동.->
	}
	else
	{
		//에러창
		ShowLoginErrorMessage(TEXT("비밀번호가 일치하지 않습니다."));
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Password Error"));
	}
}

void UMH_StartWidget::OnClickedBackButton()
{
	//Start ui로 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(0);
}

void UMH_StartWidget::OnClickedFANButton()
{
	//Fan으로 유저모드 설정. 관리자모드 off
	bIsHost_Signup = false;
	if (!bIsHost_Signup)
	{
		FButtonStyle ButtonStyleFAN = Btn_FAN->GetStyle();
		FSlateBrush NormalBrushFAN = ButtonStyleFAN.Normal;

		FButtonStyle ButtonStyleMANAGER = Btn_MANAGER->GetStyle();
		FSlateBrush NormalBrushMANAGER = ButtonStyleMANAGER.Normal;
		// 노멀 이미지 알파값 업데이트
		NormalBrushMANAGER.TintColor = FSlateColor(FLinearColor(0.0f , 0.0f , 0.0f , 1.0f));
		NormalBrushFAN.TintColor = FSlateColor(FLinearColor(1.0f , 1.0f , 1.0f , 1.0f));
		//Btn_FAN->SetColorAndOpacity(FLinearColor(1.0f , 1.0f , 1.0f , 1.0f)); //밝게
		//Btn_MANAGER->SetColorAndOpacity(FLinearColor(0.0f , 0.0f , 0.0f , 1.0f)); //어둡게
		// 변경된 브러쉬를 다시 스타일에 설정
		ButtonStyleMANAGER.Normal = NormalBrushMANAGER;
		ButtonStyleFAN.Normal = NormalBrushFAN;

		// 스타일을 버튼에 다시 적용
		Btn_MANAGER->SetStyle(ButtonStyleMANAGER);
		Btn_FAN->SetStyle(ButtonStyleFAN);
	}
}

void UMH_StartWidget::OnClickedMANAGERButton()
{
	//MANAGER로 유저모드 설정. 관리자모드 on
	bIsHost_Signup = true;
	if (bIsHost_Signup)
	{
		FButtonStyle ButtonStyleFAN = Btn_FAN->GetStyle();
		FSlateBrush NormalBrushFAN = ButtonStyleFAN.Normal;

		FButtonStyle ButtonStyleMANAGER = Btn_MANAGER->GetStyle();
		FSlateBrush NormalBrushMANAGER = ButtonStyleMANAGER.Normal;
		// 노멀 이미지 알파값 업데이트
		NormalBrushFAN.TintColor = FSlateColor(FLinearColor(0.0f , 0.0f , 0.0f , 1.0f));
		NormalBrushMANAGER.TintColor = FSlateColor(FLinearColor(1.0f , 1.0f , 1.0f , 1.0f));
		//Btn_MANAGER->SetColorAndOpacity(FLinearColor(1.0f , 1.0f , 1.0f , 1.0f)); //밝게
		//Btn_FAN->SetColorAndOpacity(FLinearColor(0.0f , 0.0f , 0.0f , 1.0f)); //어둡게
		// 변경된 브러쉬를 다시 스타일에 설정
		ButtonStyleMANAGER.Normal = NormalBrushMANAGER;
		ButtonStyleFAN.Normal = NormalBrushFAN;

		// 스타일을 버튼에 다시 적용
		Btn_MANAGER->SetStyle(ButtonStyleMANAGER);
		Btn_FAN->SetStyle(ButtonStyleFAN);
	}
}

//
void UMH_StartWidget::OnTextChanged(const FText& Text)
{
	FString InputText = Text.ToString();
	FString FormattedText = FormatDateInput(InputText);

	// 입력된 텍스트가 수정된 경우에만 업데이트
	if (InputText != FormattedText)
	{
		EText_SignupBirth->SetText(FText::FromString(FormattedText));
	}
}

FString UMH_StartWidget::FormatDateInput(const FString& InputText)
{
	FString CleanedText;

	// 숫자만 추출하여 새로운 문자열 생성
	for (TCHAR Character : InputText)
	{
		if (FChar::IsDigit(Character))
		{
			CleanedText.AppendChar(Character);
		}
	}

	int32 Length = CleanedText.Len();
	FString FormattedText;

	// YYYY/MM/DD 형식으로 포맷팅
	if (Length > 0 && Length <= 4)
	{
		FormattedText = CleanedText;
	}
	else if (Length > 4 && Length <= 6)
	{
		FormattedText = CleanedText.Left(4) + TEXT("/") + CleanedText.Mid(4);
	}
	else if (Length > 6)
	{
		FormattedText = CleanedText.Left(4) + TEXT("/") + CleanedText.Mid(4 , 2) + TEXT("/") + CleanedText.Mid(6);
	}

	// 최대 길이 제한 (10자리: YYYY/MM/DD)
	if (FormattedText.Len() > 10)
	{
		FormattedText = FormattedText.Left(10);
	}

	return FormattedText;
}

void UMH_StartWidget::OnClickedSelectAvatarRButton()
{
	// 아바타 이미지 오른쪽으로 이동 (순환)
	int32 PreviousCharacterModelNum = CharacterModelNum;
	CharacterModelNum = (CharacterModelNum % 4) + 1; // 1에서 4까지 순환

	// 현재 아바타는 On 애니메이션 재생
	PlayerImgOnAnim(CharacterModelNum);
	// 이전 아바타는 Off 애니메이션 재생
	PlayerImgOffAnim(PreviousCharacterModelNum);

	//애니메이션, 사진(버튼) 클릭하면 위젯 확대
}

void UMH_StartWidget::OnClickedSelectAvatarLButton()
{
	// 아바타 이미지 왼쪽으로 이동 (순환)
	int32 PreviousCharacterModelNum = CharacterModelNum;
	CharacterModelNum = (CharacterModelNum - 2 + 4) % 4 + 1; // 4에서 1로 순환

	// 현재 아바타는 On 애니메이션 재생
	PlayerImgOnAnim(CharacterModelNum);
	// 이전 아바타는 Off 애니메이션 재생
	PlayerImgOffAnim(PreviousCharacterModelNum);
}

void UMH_StartWidget::OnClickedAvatarConfirmButton()
{
	//회원가입 완료-> 로비맵으로 이동(세션 생성,입장) - >콘서트 선택UI로 이동
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		//ATTPlayer* Player = Cast<ATTPlayer>(GetOwningPlayer());
		//if (Player)
		//{
		//	Player->SetAvatarData(CharacterModelNum);
		//}
		//관리자 여부, 이메일,비번,나이,닉네임, 캐릭터Num
		HttpActor->ReqPostSignup(EText_Nickname->GetText() , bIsHost_Signup , EText_SignupEmail->GetText() ,
		                         EText_SignupPassWord->GetText() ,
		                         EText_SignupBirth->GetText().ToString() , CharacterModelNum);
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Signup Error"));
	}
	//회원가입 응답 성공이면 로비로 이동
	//GoToLobby();
}

void UMH_StartWidget::PlayerImgOnAnim(int32 AnimNum)
{
	switch (AnimNum)
	{
	case 1:
		PlayAnimation(AvatarAnim1On);
		break;
	case 2:
		PlayAnimation(AvatarAnim2On);
		break;
	case 3:
		PlayAnimation(AvatarAnim3On);
		break;
	case 4:
		PlayAnimation(AvatarAnim4On);
		break;
	default:
		break;
	}
}

void UMH_StartWidget::PlayerImgOffAnim(int32 AnimNum)
{
	switch (AnimNum)
	{
	case 1:
		PlayAnimation(AvatarAnim1Off);
		break;
	case 2:
		PlayAnimation(AvatarAnim2Off);
		break;
	case 3:
		PlayAnimation(AvatarAnim3Off);
		break;
	case 4:
		PlayAnimation(AvatarAnim4Off);
		break;
	default:
		break;
	}
}

void UMH_StartWidget::SetLoadingActive(bool bIsActive)
{
	if (bIsActive == true)
	{
		Img_Loading->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(LoadingAnim);
	}
}
