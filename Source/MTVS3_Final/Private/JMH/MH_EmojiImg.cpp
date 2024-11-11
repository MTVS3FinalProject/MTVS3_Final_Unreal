// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_EmojiImg.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UMH_EmojiImg::SetAnimMaterial(UMaterialInterface* NewAnim)
{
	IMG_Emoji->SetBrushFromMaterial(NewAnim);
}

void UMH_EmojiImg::ShowCanvas()
{
	Can_Emoji->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMH_EmojiImg::HideCanvas()
{
	Can_Emoji->SetVisibility(ESlateVisibility::Hidden);
}
