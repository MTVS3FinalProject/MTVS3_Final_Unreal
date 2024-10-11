// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_StartWidget.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor.h"

void UMH_StartWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//Login
	Btn_test_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::Test_CreateSesstion); //테스트 세션생성 버튼
	Btn_Exit_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedExitButton);
	Btn_AddPicture->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedAddPictureButton);
	Btn_ForgotPassword_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedForgotPasswordButton);
	//이부분 확인해보기, 로그인 함수 하나 더 만들어서 그안에서 세션 생성해야함
	Btn_GoToLobby_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSignInButton);
	Btn_SignUp_Login->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSignUpButton);
	//Signup
	Btn_Back_Signup->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBackButton);
	Btn_Confirm_Signup->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirmSignupButton);
	Btn_FAN->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedFANButton);
	Btn_MANAGER->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedMANAGERButton);
	Btn_SelectAvatarL->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarLButton);
	Btn_SelectAvatarR->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarRButton);
	Btn_GenderMale->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedGenderMaleButton);
	Btn_GenderFeMale->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedGenderFeMaleButton);
	//Avatar
	Btn_GoToLobby_Avatar->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedAvatarConfirmButton);

	//QR1
	Btn_Confirm_QRUi1->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirm_QRUi1Button);
	Btn_Back_QRUi1->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBack_QRUi1Button);
	//QR2
	Btn_Confirm_QRUi2->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirm_QRUi2Button);


	//나이 설정 1~100
	if (Com_SetAge)
	{
		for (int32 i = 1; i <= 100; i++)
		{
			// 숫자를 문자열로 변환하여 ComboBoxString에 추가
			Com_SetAge->AddOption(FString::FromInt(i));
		}
	}
}

void UMH_StartWidget::Test_CreateSesstion()
{
	GoToLobby();
}

void UMH_StartWidget::GoToLobby()
{
	//로비맵으로 이동(세션 생성,입장)
	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Create Lobby Session"));
}

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
	GoToLobby();
}

void UMH_StartWidget::OnClickedSignUpButton()
{
	//회원가입 스위쳐 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMH_StartWidget::OnClickedExitButton()
{
	//게임 종료
}

void UMH_StartWidget::OnClickedForgotPasswordButton()
{
	//비번찾기
}

void UMH_StartWidget::OnClickedConfirm_QRUi1Button()
{
	//확인 버튼 누르면(얼굴인식 확인)->
	//확인완료 ui로->
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(4);
}

void UMH_StartWidget::OnClickedBack_QRUi1Button()
{
	//회원가입으로 다시 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMH_StartWidget::SetQRImg(UTexture2D* newTexture)
{
	//QR 받아오기
	Img_QR->SetBrushFromTexture(newTexture);
}

void UMH_StartWidget::OnClickedConfirm_QRUi2Button()
{
	//확인버튼 누르면 ->
	//아바타 설정ui로 이동.
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(0);
	
}



void UMH_StartWidget::OnClickedConfirmSignupButton()
{
	//회원가입 완료 -> 유저정보 서버로 전달
	//비번 중복확인.
	//FText 끼리 비교? string 으로 비교?
	FText Password1 = EText_SignupPassWord->GetText();
	FText Password2 = EText_SignupPassWord2->GetText();

	if (Com_SetAge)
	{
		FString SelectedOption = Com_SetAge->GetSelectedOption();
		Age_SelectedValue = FCString::Atoi(*SelectedOption); // 문자열을 int로 변환
	}
	//모두 입력했는지 확인
	//아바타 설정으로 이동

	//비밀번호 같은지 확인
	if (Password1.EqualTo(Password2))
	{

		//사진확인 QR로 이동
		//WS_StartWidgetSwitcher->SetActiveWidgetIndex(2);
		//
		//사진은 어케 불러와->
		//QR띄워주는 ui로 이동.->
		WS_StartWidgetSwitcher->SetActiveWidgetIndex(3);
		//QR이미지 받아오기 ->
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Password!!!"));
	}
	else
	{
		//에러창
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
	//bIsHost_Signup = false;
}

void UMH_StartWidget::OnClickedMANAGERButton()
{
	//MANAGER로 유저모드 설정. 관리자모드 on
	//bIsHost_Signup = true;
}

void UMH_StartWidget::OnClickedAddPictureButton()
{
	//지워도됨?
	//사진 서버로 전달
	//사진 정상적인지 확인
}

void UMH_StartWidget::OnClickedSelectAvatarRButton()
{
	// 아바타 이미지 오른쪽 이미지로
		CharacterModelNum++;
	if (CharacterModelNum > 4)
	{
		CharacterModelNum = 0;
	}
	//애니메이션, 사진(버튼) 클릭하면 위젯 확대
}

void UMH_StartWidget::OnClickedSelectAvatarLButton()
{
	//아바타 이미지 왼쪽이미지로
	CharacterModelNum--;

	if (CharacterModelNum < 0)
	{
		CharacterModelNum = 4;
	}
	//애니메이션, 사진(버튼) 클릭하면 위젯 확대
	
}

//삭제
void UMH_StartWidget::OnClickedGenderMaleButton()
{
	//Gender == Male
}

void UMH_StartWidget::OnClickedGenderFeMaleButton()
{
	//Gender == FeMale
}
//

void UMH_StartWidget::OnClickedAvatarConfirmButton()
{
	//회원가입 완료-> 로비맵으로 이동(세션 생성,입장) - >콘서트 선택UI로 이동
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		//관리자 여부, 이메일,비번,나이,닉네임, 캐릭터Num
		HttpActor->ReqPostSignup(bIsHost_Signup , EText_SignupEmail->GetText() , EText_SignupPassWord->GetText() ,
		                         Age_SelectedValue , EText_Nickname->GetText() , CharacterModelNum);
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Signup Error"));
	}
//회원가입 응답 성공이면 로비로 이동
	GoToLobby();
}

//삭제
void UMH_StartWidget::OnClickedAvatarStyleAButton()
{
	//여자캐릭 or 남자캐릭?
}

void UMH_StartWidget::OnClickedAvatarStyleBButton()
{
	//여자캐릭 or 남자캐릭?
}
//