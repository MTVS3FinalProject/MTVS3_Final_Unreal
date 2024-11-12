// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_EmojiImg.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UMH_EmojiImg::NativeConstruct()
{
	Super::NativeConstruct();
	HideCanvas();
}

void UMH_EmojiImg::SetMaterial(UMaterialInterface* NewAnim)
{
	Img_Emoji->SetBrushFromMaterial(NewAnim);
}

void UMH_EmojiImg::AnimMaterialSwitcher(int32 num)
{
	switch (num)
	{
	case 1 : SetMaterial(HappyAnim);
		break;

	case 2 : SetMaterial(KissAnim);
		break;

	case 3 : SetMaterial(LoveAnim);
		break;

	case 4 : SetMaterial(SadAnim);
		break;
		
	case 5 : SetMaterial(SurprisedAnim);
		break;

		default: break;
	}
}

void UMH_EmojiImg::ShowCanvas()
{
	PlayAnim();
	Can_Emoji->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMH_EmojiImg::HideCanvas()
{
	Can_Emoji->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_EmojiImg::PlayAnim()
{
	PlayAnimation(EmojiVisibleAnim);
}
