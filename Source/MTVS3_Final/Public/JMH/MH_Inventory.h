// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Inventory.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTitleData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 TitleID;

	UPROPERTY(BlueprintReadOnly)
	FString TitleName;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* TitleImg;
};

USTRUCT(BlueprintType)
struct FTicketData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 StickerID;

	UPROPERTY(BlueprintReadOnly)
	FString StickerScript;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* StickerImg;
};

USTRUCT(BlueprintType)
struct FStickerData
{
	GENERATED_BODY()
		
	UPROPERTY(BlueprintReadOnly)
	FString TicketID;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* TicketImage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBack_Inven);

UCLASS()
class MTVS3_FINAL_API UMH_Inventory : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnClickedBack_Inven OnClickedBack_InvenBtn;

	bool bIsCollectionBookVisible = false;
	
	UFUNCTION()
	void CloseBtn_Inven()
	{
		//상태 바꿔주고
		bIsCollectionBookVisible = !bIsCollectionBookVisible;
		//끄기
		OnClickedBack_InvenBtn.Broadcast();
	}
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_InvenWidgetSwitcher;
	//위젯스위처설정 
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);//0:칭호,1:티켓,2:스티커
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back_Inven;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_00_PlayerTitle;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_01_Ticket;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_02_Sticker;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_00_PlayerTitle;
	UFUNCTION()
	void OnClicked_PlayerTitle();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_01_Ticket;
	UFUNCTION()
	void OnClicked_Ticket();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_02_Sticker;
	UFUNCTION()
	void OnClicked_Sticker();
	
	//칭호,티켓,스티커 정보 받아오기
	UFUNCTION()
	void SetPlayerTitleInfo();
	UFUNCTION()
	void SetPlayerTicketsInfo();
	UFUNCTION()
	void SetPlayerStickerInfo();

	//플레이어 칭호 적용
	UFUNCTION()
	void SetPlayerTitle(int32 TitleID);

	//정보창 띄우기
	UFUNCTION()
	void ShowInfoWin();

	
};
