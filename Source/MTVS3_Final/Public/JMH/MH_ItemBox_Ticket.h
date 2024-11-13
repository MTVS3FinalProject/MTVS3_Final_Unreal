// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Ticket.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Ticket : public UUserWidget
{
	GENERATED_BODY()

	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Ticket;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Ticket;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Ticket;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_SeatInfo;

	//UFUNCTION()
	//void SetTicketData(const FTicketItemData& InTicketData);
	
	UFUNCTION()
	void OnClickedTicketBtn();
	
	
};
