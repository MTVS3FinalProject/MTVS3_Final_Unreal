// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MainWidget.h"

#include "AssetTypeCategories.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibleCanvas(true);

	Btn_BackMain->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBackMain);

}

void UMainWidget::SetVisibleCanvas(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}
	
	else if(!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::SetTextCurrentTime(FString CurrentTime)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_CurrentTime->SetText(FText::FromString(CurrentTime));
	
}

void UMainWidget::OnClickedBackMain()
{
	// 방에서 퇴장하고 싶다.
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->ExitSession();
	}
}
