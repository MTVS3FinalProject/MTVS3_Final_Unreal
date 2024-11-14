// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Ticket.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered_Ticket, bool, bHovered);
UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Ticket : public UUserWidget
{
	GENERATED_BODY()

public:
	// 아이템 박스 버튼에 마우스 호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Ticket OnItemHovered_Ticket;
	
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
	
private:
	UFUNCTION()
	void OnButtonHovered_Ticket();
};
