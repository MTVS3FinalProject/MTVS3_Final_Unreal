// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "NoesisSDK.h"
//#include "NsApp/RiveControl.h"
#include "HM_MainBarWidget.generated.h"

/**
 * 
 */

USTRUCT()
struct FMenuButtonGroup
{
	GENERATED_BODY()

	UPROPERTY()
	class UButton* Button = nullptr;

	UPROPERTY()
	class UImage* BtnImg = nullptr;

	UPROPERTY()
	class UImage* BtnImg2 = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedChatBtn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedGoToTutorialBtn);

UCLASS()
class MTVS3_FINAL_API UHM_MainBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TArray<FMenuButtonGroup> MenuButtonGroups;

	//Rive
	// Rive 트리거를 활성화하는 함수
	//UFUNCTION(BlueprintCallable, Category="Rive")
	//void ActivateRiveTrigger(const FString& TriggerName);

	// Rive 매개변수를 설정하는 함수
	//UFUNCTION(BlueprintCallable, Category = "Rive")
	//void SetRiveParameter(const FString& ParameterName, float Value);

private:
	// XAML에서 로드된 RootElement (RiveControl이 포함됨)
	//Noesis::Ptr<Noesis::FrameworkElement>RootElement;
	// RiveControl의 참조를 저장
	//NoesisApp::RiveControl* RiveControl;
	// Unreal의 콘텐츠 경로 기준으로 XAML 파일 경로를 설정
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rive", meta = (AllowPrivateAccess = "true"))
	//FString XamlFilePath;

public:
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_Bar;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UMH_Inventory* WBP_inventoryUI;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UMH_NoticeWidget* WBP_NoticeUI;

	UFUNCTION()
	void SetVisibleSwitcher(bool bVisible);

	UFUNCTION()
	void SetWidgetSwitcher(int32 num); //0:Emoji . 01: Notice . 02: CollectionBook . 03: Settings

	UFUNCTION()
	void SetVisibilityState();

	bool bIsEmojiVisible = false;
	bool bIsCollectionBookVisible = false;
	bool bIsNoticeVisible = false;
	bool bIsMenuVisible = false;
	bool bIsChatVisible = false;
	bool bIsSettingsVisible = false;

	FOnClickedChatBtn OnClickedShowChatBtn;

	UFUNCTION()
	void CloseButtonPressed()
	{
		//채팅이 켜져있지 않으면
		if (!bIsChatVisible)
		{
			//모든 카테고리를 끄고
			CloseAllCategory();
		}
		//상태 바꿔주고
		bIsChatVisible = !bIsChatVisible;
		// 델리게이트 호출. 채팅 켜기
		OnClickedShowChatBtn.Broadcast();
		if (bIsChatVisible)
		{
			//기본 배경 보이게
			OnClickedMenuBtn(Btn_Chat);
		}
	}


	//MH
	/*	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
		class UButton* Btn_lightMode;
		UFUNCTION()
		void OnClickedlightModeBtn();
	
		UPROPERTY(VisibleAnywhere , meta=(BindWidget))
		class UButton* Btn_DarkMode;
		UFUNCTION()
		void OnClickedDarkModeBtn();
	
		// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
		// class UButton* Btn_Menu;
		// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
		// class UCanvasPanel* Can_0_Menu;
		// UFUNCTION()
		// void OnClickedMenuBtn();
	
		//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
		//class UButton* Btn_Emoji;
		//UFUNCTION()
		//void OnClickedEmojiBtn();
		*/

	//메뉴바 버튼 이미지
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_NoticeBtn;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_CollecBtn;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_ChatBtn;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_SettingBtn;

	//메뉴바 버튼 이미지2
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* img_BaseBar;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_NoticeBG;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_CollecBG;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_ChatBG;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_SettingBG;

	UFUNCTION()
	void SetVisibleMenuBG(bool Visible , UImage* ImgBG);

	UFUNCTION()
	void OnHoveredMenuBtn(bool bIsHovere , UButton* OnHoveredButton);

	UFUNCTION()
	void OnClickedMenuBtn(UButton* OnClickedButton);
	UFUNCTION()
	void InitMenuBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Image_Notice;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Notice;
	UFUNCTION()
	void OnClickedNoticeBtn();
	UFUNCTION()
	void OnHoveredNoticeBtn();
	UFUNCTION()
	void OnUnHoveredNoticeBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_CollectionBook;
	UFUNCTION()
	void OnClickedCollectionBookBtn();
	UFUNCTION()
	void OnHoveredCollectionBookBtn();
	UFUNCTION()
	void OnUnHoveredCollectionBookBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Chat;
	UFUNCTION()
	void OnClickedChatBtn();
	UFUNCTION()
	void OnHoveredChatBtn();
	UFUNCTION()
	void OnUnHoveredChatBtn();


	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Setting;
	UFUNCTION()
	void OnClickedSettingBtn();
	UFUNCTION()
	void OnHoveredSettingBtn();
	UFUNCTION()
	void OnUnHoveredSettingBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Settings;
	UFUNCTION()
	void OnClickedSettingBackBtn();

	UFUNCTION()
	void CloseAllCategory();

	// KHJ
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class USlider* Slider_BG;
	UFUNCTION()
	void OnVolumeChanged(float Value);
	UPROPERTY()
	float CurrentVolume = 0.75f;

	UPROPERTY(BlueprintAssignable , Category = "Events")
	FOnClickedGoToTutorialBtn OnClickedShowTutorialBtn;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_GoToTutorial;
	UFUNCTION()
	void OnClickedGoToTutorialBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Credit;
	UFUNCTION()
	void OnClickedCreditBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Credit;
	UFUNCTION()
	void OnClickedCreditBackBtn();
};
