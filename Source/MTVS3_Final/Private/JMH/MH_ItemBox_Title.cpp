// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Title.h"

#include "Components/Button.h"

void UMH_ItemBox_Title::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Item_Title->OnClicked.AddDynamic(this,&UMH_ItemBox_Title::OnClickedTitle);
}

//void UMH_ItemBox_Title::SetTitleData(const FTitleItemData& InTitleData)
//{


	
//}
/*
FReply UMH_ItemBox_Title::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("111111!"));
	Super::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	// 더블 클릭 이벤트가 발생했을 때 델리게이트 호출
	OnDoubleClicked.Broadcast(this);

	return FReply::Handled();
}*/
