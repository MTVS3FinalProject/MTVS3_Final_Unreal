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
	void NativeConstruct() override;
	
	//Start
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WS_StartWidgetSwitcher;
	
	//Login
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Can_Login;

	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_SignIn;

	UFUNCTION()
	void OnClickedSignInButton();
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_SignUp;

	UFUNCTION()
	void OnClickedSignUpButton();
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_Exit;

	UFUNCTION()
	void OnClickedExitButton();
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_ForgotPassword;

	UFUNCTION()
	void OnClickedForgotPasswordButton();

	UPROPERTY(meta=(BindWidget))
	class UEditableText* EText_Email;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText* EText_PassWord;

	//SignUp
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Can_SignUp;

	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_Confirm;
	
	UFUNCTION()
	void OnClickedConfirmButton();
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_Back;
	
	UFUNCTION()
	void OnClickedBackButton();

	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_FAN;
	
	UFUNCTION()
	void OnClickedFANButton();
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_MANAGER;
	
	UFUNCTION()
	void OnClickedMANAGERButton();
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString*  Com_SetAge;
	
	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_AddPicture;

	UFUNCTION()
	void OnClickedAddPictureButton();
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText* EText_SignupEmail;
		
	UPROPERTY(meta=(BindWidget))
	class UEditableText* EText_SignupPassWord;


	//Select Avatar

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Can_Avatar;
	
};
