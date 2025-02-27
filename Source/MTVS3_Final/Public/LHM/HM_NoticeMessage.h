// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_NoticeMessage.generated.h"

/**
 * 
 */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageClicked, int32, ClickedMailId);

UCLASS()
class MTVS3_FINAL_API UHM_NoticeMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Message;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Message;
	
	// 클릭 이벤트 처리
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnMessageClicked OnMessageClicked;

	int32 MailId;
	void SetMailId(int32 InMailId) { MailId = InMailId; };
	FString MailCategory;
	void SetMailCategory(FString InMailCategory) { MailCategory = InMailCategory; };

	UFUNCTION()
	void HandleMessageClicked()
	{
		UE_LOG(LogTemp, Log, TEXT("Button clicked! MailId: %d"), MailId);
		UE_LOG(LogTemp, Log, TEXT("Button clicked! MailCategory: %s"), *MailCategory);
		OnMessageClicked.Broadcast(MailId);
	}
};
