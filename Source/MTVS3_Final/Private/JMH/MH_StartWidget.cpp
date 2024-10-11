// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_StartWidget.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor.h"

void UMH_StartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Back->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedBackButton);
	Btn_Confirm->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedConfirmButton);
	Btn_Exit->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedExitButton);
	Btn_AddPicture->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedAddPictureButton);
	Btn_ForgotPassword->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedForgotPasswordButton);
	Btn_GoToLobby_SignIn->OnClicked.AddDynamic(this , &UMH_StartWidget::GoToLobby);
	Btn_SignUp->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSignUpButton);
	Btn_FAN->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedFANButton);
	Btn_MANAGER->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedMANAGERButton);
	Btn_SelectAvatarL->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarLButton);
	Btn_SelectAvatarR->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedSelectAvatarRButton);
	Btn_GenderMale->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedGenderMaleButton);
	Btn_GenderFeMale->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedGenderFeMaleButton);
	Btn_GoToLobby_Avatar->OnClicked.AddDynamic(this , &UMH_StartWidget::OnClickedAvatarConfirmButton);

	if (Com_SetAge)
	{
		for (int32 i = 1; i <= 100; i++)
		{
			// 숫자를 문자열로 변환하여 ComboBoxString에 추가
			Com_SetAge->AddOption(FString::FromInt(i));
		}
	}
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
		HttpActor->ReqPostLogin(EText_Email->GetText(),EText_PassWord->GetText());
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Login Error"));
	}
	//로비로 이동
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

void UMH_StartWidget::OnClickedConfirmButton()
{
	//회원가입 완료 -> 유저정보 서버로 전달
	//이메일, 비번 중복확인
	//사진확인
	//모두 입력했는지 확인
	//아바타 설정으로 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(2);
	
	if (Com_SetAge)
	{
		FString SelectedOption = Com_SetAge->GetSelectedOption();
		Age_SelectedValue = FCString::Atoi(*SelectedOption); // 문자열을 int로 변환
	}
	
		//아바타 설정으로 이동
		WS_StartWidgetSwitcher->SetActiveWidgetIndex(2);
		//사진은 어케 불러와

}

void UMH_StartWidget::OnClickedBackButton()
{
	//Start ui로 이동
	WS_StartWidgetSwitcher->SetActiveWidgetIndex(0);
}

void UMH_StartWidget::OnClickedFANButton()
{
	//Fan으로 유저모드 설정
}

void UMH_StartWidget::OnClickedMANAGERButton()
{
	//MANAGER로 유저모드 설정
}

void UMH_StartWidget::OnClickedAddPictureButton()
{
	//사진 서버로 전달
	//사진 정상적인지 확인
}

void UMH_StartWidget::OnClickedSelectAvatarRButton()
{
	// 아바타 이미지 오른쪽 이미지로
}

void UMH_StartWidget::OnClickedSelectAvatarLButton()
{
	//아바타 이미지 왼쪽이미지로
}

void UMH_StartWidget::OnClickedGenderMaleButton()
{
	//Gender == Male
}

void UMH_StartWidget::OnClickedGenderFeMaleButton()
{
	//Gender == FeMale
}

void UMH_StartWidget::OnClickedAvatarConfirmButton()
{
	//회원가입 완료-> 로비맵으로 이동(세션 생성,입장) - >콘서트 선택UI로 이동
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		//관리자 여부, 이메일,비번,나이,닉네임, 캐릭터Num
		HttpActor->ReqPostSignup(bIsHost_Signup,EText_SignupEmail->GetText(),EText_SignupPassWord->GetText(),Age_SelectedValue ,EText_Nickname->GetText(),CharacterModelNum);
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("Signup Error"));
	}
	
	GoToLobby();
}

void UMH_StartWidget::OnClickedAvatarStyleAButton()
{
	//여자캐릭 or 남자캐릭?
}

void UMH_StartWidget::OnClickedAvatarStyleBButton()
{
	//여자캐릭 or 남자캐릭?
}
