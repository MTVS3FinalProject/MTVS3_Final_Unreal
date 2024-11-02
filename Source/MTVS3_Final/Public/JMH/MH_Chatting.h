// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_TTHUD.h"
#include "Blueprint/UserWidget.h"
#include "MH_Chatting.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_Chatting : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	//엔터키 누르면 채팅창 활성화
	UFUNCTION()
	void ActivateChatText();

	UPROPERTY(meta = (BindWidget))
	class UEditableText* EText_Chat;
	
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Scroll_Chat;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Send;
	UFUNCTION()
	void OnClickedSendBtn();
	
	//엔터키 한번 더 누르면 입력 내보냄

	UFUNCTION()
	void AddChatMessage(const FString& Message);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMH_Message> MessageWidget;
	
protected:
	
	UPROPERTY()
	APlayerController* OwningPlayer;
	
};
