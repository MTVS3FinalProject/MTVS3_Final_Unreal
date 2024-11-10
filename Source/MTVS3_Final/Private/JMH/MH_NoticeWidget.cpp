// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_NoticeWidget.h"

#include "Components/Button.h"

void UMH_NoticeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Back_Notice->OnClicked.AddDynamic(this,&UMH_NoticeWidget::CloseBtn_Notice);
}
