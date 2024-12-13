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

private:

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
		// 델리게이트 호출. 채팅
		OnClickedShowChatBtn.Broadcast();
	}

	UFUNCTION()
	void OnclickedMenuBtn_chat();
	UFUNCTION()
	void SetIsvisible_chat();

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
	UFUNCTION()
	void CloseAllCategory_Chat();

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
