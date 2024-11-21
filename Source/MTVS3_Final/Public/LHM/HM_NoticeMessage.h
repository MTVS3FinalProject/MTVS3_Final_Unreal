// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_NoticeMessage.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_NoticeMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	int32 MailId;
	void SetMailId(int32 InMailId) { MailId = InMailId; };
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Message;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Message;
	
	// 클릭 이벤트 처리
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageClicked, int32, ClickedMailId);
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnMessageClicked OnMessageClicked;
	void HandleMessageClicked();
};
