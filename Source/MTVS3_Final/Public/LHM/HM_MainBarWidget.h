// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_MainBarWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedChatBtn);

UCLASS()
class MTVS3_FINAL_API UHM_MainBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

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
	}
	

	//MH
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_lightMode;
	UFUNCTION()
	void OnClickedlightModeBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_DarkMode;
	UFUNCTION()
	void OnClickedDarkModeBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Menu;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_0_Menu;
	UFUNCTION()
	void OnClickedMenuBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Emoji;
	UFUNCTION()
	void OnClickedEmojiBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Notice;
	UFUNCTION()
	void OnClickedNoticeBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_CollectionBook;
	UFUNCTION()
	void OnClickedCollectionBookBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Chat;
	UFUNCTION()
	void OnClickedChatBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Setting;
	UFUNCTION()
	void OnClickedSettingBtn();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Settings;
	UFUNCTION()
	void OnClickedSettingBackBtn();

	UFUNCTION()
	void CloseAllCategory();


#pragma region 현민 Http Test용
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_HttpTest01;
	UFUNCTION()
	void OnClickedHttpTest01();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_HttpTest02;
	UFUNCTION()
	void OnClickedHttpTest02();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_HttpTest03;
	UFUNCTION()
	void OnClickedHttpTest03();
#pragma endregion
};
