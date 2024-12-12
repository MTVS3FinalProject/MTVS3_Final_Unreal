// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NoesisRuntime.h"
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

	//Rive
	// Rive 트리거를 활성화하는 함수
	UFUNCTION(BlueprintCallable, Category="Rive")
	void ActivateRiveTrigger(const FString& TriggerName);

	// Rive 매개변수를 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Rive")
	void SetRiveParameter(const FString& ParameterName, float Value);

private:
	// XAML에서 로드된 RootElement (RiveControl이 포함됨)
	Noesis::Ptr<Noesis::FrameworkElement>RootElement;
	// RiveControl의 참조를 저장
	Noesis::DependencyObject* RiveControl;
	// Unreal의 콘텐츠 경로 기준으로 XAML 파일 경로를 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rive", meta = (AllowPrivateAccess = "true"))
	FString XamlFilePath;
	
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
	}
	

	//MH
/*	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_lightMode;
	UFUNCTION()
	void OnClickedlightModeBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_DarkMode;
	UFUNCTION()
	void OnClickedDarkModeBtn();*/

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

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Image_Notice;
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

};
