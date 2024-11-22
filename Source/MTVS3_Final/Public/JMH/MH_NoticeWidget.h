// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_NoticeWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBack_Notice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedPaymentPostpone);

UCLASS()
class MTVS3_FINAL_API UMH_NoticeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Notice;
	FOnClickedBack_Notice OnClickedBack_NoticeBtn;
	//cpp에서 버튼 바인드해주기 
	UFUNCTION()
	void CloseBtn_Notice()
	{
		//끄기
		OnClickedBack_NoticeBtn.Broadcast();
	}

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Payment;
	FOnClickedPaymentPostpone OnClickedPaymentPostpone;
	//cpp에서 버튼 바인드해주기 
	UFUNCTION()
	void Payment_Postpone()
	{
		// 우편함 끄고 미룬 결제 하러가기
		OnClickedBack_NoticeBtn.Broadcast();
		OnClickedPaymentPostpone.Broadcast();
	}
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UVerticalBox* Vertical_MessageBox;
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TSubclassOf<class UHM_NoticeMessage> NoticeMessageFac;
	
	// 메세지 생성 및 초기화 함수
	UFUNCTION()
	void InitializeMessageTabs();
	UFUNCTION()
	void OnMessageSelected(int32 MailId);
	UFUNCTION()
	void OnPostponeMessageSelected(int32 MailId);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Canvas_content;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Subject;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Content;
	
	UFUNCTION()
	void OnMailDetailReceived(FString Subject, FString Content);
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Content;
	UFUNCTION()
	void CloseBtn_Content();
};
