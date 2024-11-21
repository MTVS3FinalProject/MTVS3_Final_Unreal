// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ItemBox_Title.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedTitleBtn, UMH_ItemBox_Title*, ClickedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHovered_Title,UMH_ItemBox_Title*, HoveredItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnHovered_Title,UMH_ItemBox_Title*, UnHoveredItem);

UCLASS()
class MTVS3_FINAL_API UMH_ItemBox_Title : public UUserWidget
{
	GENERATED_BODY()

public:
	// 아이템 박스 버튼에 마우스 호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Title OnItemHovered_Title;
	
	UFUNCTION()
	void OnButtonHovered_Title()
	{
		OnItemHovered_Title.Broadcast(this);
	}

	// 아이템 박스 버튼에 마우스 언호버 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnItemHovered_Title OnItemUnHovered_Title;
	
	UFUNCTION()
	void OnButtonUnHovered_Title()
	{
		OnItemUnHovered_Title.Broadcast(this);
	}

	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Item_Title;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Item_Title;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Title;

	//현민
	//타이틀 ID값 저장하고 해제할때 필요
	UPROPERTY()
	int32 TitleID=0;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnClickedTitleBtn OnClickedTitleBtn;

	UFUNCTION()
	void OnClickedTitle()
	{
		OnClickedTitleBtn.Broadcast(this);
	}

public:

	UFUNCTION()
	void ChangeColorTitleName(const FString& _TitleRarity);

	//타이틀ID 값 넘겨주기
	UFUNCTION()
	void SetTitleID(int32 TitleID0);
	
	UFUNCTION()
	int32 GetTitleID();
	
	//타이틀 등급
	UFUNCTION()
	void SetTitleRarity(FString TitleRarity);
	
	UFUNCTION()
	FString GetTitleRarity();
	
	UPROPERTY()
	FString RarityString_Title;

    //타이틀 설명
	UPROPERTY()
	FString InfoTextString_Title;
	
	UFUNCTION()
	FString GetInfoString_Title();

	UFUNCTION()
	void SetInfoString_Title(FString infoString_Title);
	
};
