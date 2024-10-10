// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	class UWidgetSwitcher* WS_StartWidgetSwitcher;
	
	//Login
	
	UFUNCTION()
	void GoToLobby();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Login;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GoToLobby_SignIn;

	UFUNCTION()
	void OnClickedSignInButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_SignUp;

	UFUNCTION()
	void OnClickedSignUpButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Exit;

	UFUNCTION()
	void OnClickedExitButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_ForgotPassword;

	UFUNCTION()
	void OnClickedForgotPasswordButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_Email;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_PassWord;

	//SignUp
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_SignUp;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Confirm;
	
	UFUNCTION()
	void OnClickedConfirmButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back;
	
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
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UComboBoxString*  Com_SetAge;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_AddPicture;

	UFUNCTION()
	void OnClickedAddPictureButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupEmail;
		
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_SignupPassWord;


	//Select Avatar

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Avatar;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Avatar;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Step1;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Step2;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Step3;
	
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
	class UButton*  Btn_GenderMale;
		
	UFUNCTION()
	void OnClickedGenderMaleButton();
		
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GenderFeMale;

	UFUNCTION()
	void OnClickedGenderFeMaleButton();
			
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GoToLobby_Avatar;
	
	UFUNCTION()
	void OnClickedAvatarConfirmButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_AvatarStyleA;
	
	UFUNCTION()
	void OnClickedAvatarStyleAButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_AvatarStyleB;
	
	UFUNCTION()
	void OnClickedAvatarStyleBButton();
	
	
};
