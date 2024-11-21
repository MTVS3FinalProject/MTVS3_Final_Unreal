// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Ticket.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered_Ticket , UMH_ItemBox_Ticket* , HoveredItem_Ticket);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnHovered_Ticket , UMH_ItemBox_Ticket* , UnHoveredItem_Ticket);

UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Ticket : public UUserWidget
{
	GENERATED_BODY()

public:
	// 아이템 박스 버튼에 마우스 호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Ticket OnItemHovered_Ticket;

	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Ticket OnItemUnHovered_Ticket;
	
	UFUNCTION()
	void OnButtonHovered_Ticket()
	{
		OnItemHovered_Ticket.Broadcast(this); // 델리게이트 호출
	}
	
	UFUNCTION()
	void OnButtonUnHovered_Ticket()
	{
		OnItemUnHovered_Ticket.Broadcast(this); // 델리게이트 호출
	}
	
public:
	virtual void NativeConstruct() override;
	
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Ticket;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Ticket;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_TicketName;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_SeatInfo;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Can_Ticket;

	//UFUNCTION()
	//void SetTicketData(const FTicketItemData& InTicketData);
	
	UFUNCTION()
	void OnClickedTicketBtn();
	UFUNCTION()
	void ShowInfo_Ticket();
	UFUNCTION()
	void HideInfo_Ticket();

	//티켓 좌석 정보
	UPROPERTY()
	FString TextNameString_Ticket;

	UPROPERTY()
	FString InfoTextString_Ticket;
	
	UFUNCTION()
	void SetInfoString_Ticket(FString NameString_Ticket,FString infoString_Ticket);
	
	UFUNCTION()
	FString GetInfoString_Ticket();
	
	UFUNCTION()
	FString GetInfoNameString_Ticket();
	
};
