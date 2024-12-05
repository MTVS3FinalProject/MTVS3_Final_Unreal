// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TreeTicketBoxWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTicketClicked, int32, ClickedTicketId);

UCLASS()
class MTVS3_FINAL_API UHM_TreeTicketBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Ticket;
	
	UPROPERTY(EditAnywhere)
	int32 TicketId;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Ticket;

	// 클릭 이벤트 처리
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnTicketClicked OnTicketClicked;
	
	UFUNCTION()
	void HandleTicketClicked()
	{
		UE_LOG(LogTemp, Log, TEXT("Button clicked! TicketId: %d"), TicketId);
		OnTicketClicked.Broadcast(TicketId);
	}
};
