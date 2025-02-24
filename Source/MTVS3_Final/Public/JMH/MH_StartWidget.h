﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_ErrorMessage.h"
#include "Blueprint/UserWidget.h"
#include "MH_StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_StartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	
	//Start
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_StartWidgetSwitcher;//0:로그인, 1:회원가입,2:아바타선택,3:QR1,4:QR2,5:종료

   //Error Message
	UPROPERTY(meta=(BindWidget))
	UMH_ErrorMessage*  WBP_MH_ErrorMessage01;
	
	//Login
	UFUNCTION()
	void ShowLoginErrorMessage(FString ErrorMS);
	
	UFUNCTION()
	void HideLoginErrorMessage();
	
	// KHJ
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Loading;
	
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* LoadingAnim;
	// KHJ

	//아이디 비번 비교없이 바로 세션 접속하도록 버튼 생성 -> 테스트모드
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_test_Login;

	UFUNCTION()
	void Test_CreateSesstion();
	
	UFUNCTION()
	void GoToLobby();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Login;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GoToLobby_Login;

	UFUNCTION()
	void OnClickedSignInButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_SignUp_Login;

	UFUNCTION()
	void OnClickedSignUpButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Exit_LoginWin;
	UFUNCTION()
	void OnClickedExitWinButton();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_ExitLogin;
	UFUNCTION()
	void OnClickedExitButton();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_BackLogin;


	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_ForgotPassword_Login;

	UFUNCTION()
	void OnClickedForgotPasswordButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_Email;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_PassWord;

	//QR
	//서버한테 이미지 받아오는부분
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_QRUi1;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Confirm_QRUi1;

	UFUNCTION()
	void OnClickedConfirm_QRUi1Button();

	UFUNCTION(BlueprintCallable)
	void SetWidgetSwitcher(int32 num);

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back_QRUi1;

	UFUNCTION()
	void OnClickedBack_QRUi1Button();
	
	UPROPERTY(VisibleAnywhere)
	class UTexture2D* QR_Texture;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_QR;

	UFUNCTION()
	void SetQRImg(class UTexture2D* newTexture);
	
	
	//서버(신원인증 완료시)
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_QRUi2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Confirm_QRUi2;

	UFUNCTION()
	void OnClickedConfirm_QRUi2Button();
	
	//SignUp
	
	//관리자 모드 off
	bool bIsHost_Signup = false;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_SignUp;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Confirm_Signup;
	
	UFUNCTION()
	void OnClickedConfirmSignupButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back_Signup;
	
	UFUNCTION()
	void OnClickedBackButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_FAN;
	
	UFUNCTION()
	void OnClickedFANButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_MANAGER;
	
	UFUNCTION()
	void OnClickedMANAGERButton();
	
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UComboBoxString*  Com_SetAge;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupEmail;
		
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupPassWord;
		
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupPassWord2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupBirth;

	UFUNCTION()
	void OnTextChanged(const FText& Text);

	UFUNCTION()
	FString FormatDateInput(const FString& InputText);

	//Select Avatar
	//아바타 버튼을 클릭하면 확대.. 그담엔?
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton*  Btn_AvatarImg;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Avatar;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Avatar1;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Avatar2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Avatar3;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Avatar4;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_SelectAvatarR;

	UFUNCTION()
	void OnClickedSelectAvatarRButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_SelectAvatarL;
	
	UFUNCTION()
	void OnClickedSelectAvatarLButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_Nickname;
			
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_Avatar;
	
	UFUNCTION()
	void OnClickedAvatarConfirmButton();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 CharacterModelNum = 1;

	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim1On;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim2On;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim3On;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim4On;

	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim1Off;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim2Off;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim3Off;
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* AvatarAnim4Off;
	
	UFUNCTION()
	void PlayerImgOnAnim(int32 AnimNum);

	UFUNCTION()
	void PlayerImgOffAnim(int32 AnimNum);

	// KHJ
	UFUNCTION()
	void SetLoadingActive(bool bIsActive);
	
	
	
};
