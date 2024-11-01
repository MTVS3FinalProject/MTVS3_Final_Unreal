// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	class AMH_ChatManager* ChatManager;
	//엔터키 한번 더 누르면 입력 내보냄
protected:
	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	
	
};
