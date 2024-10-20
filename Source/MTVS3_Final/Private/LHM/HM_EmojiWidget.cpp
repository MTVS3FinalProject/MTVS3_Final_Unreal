// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_EmojiWidget.h"
#include "Components/Button.h"

void UHM_EmojiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Emotion1->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion1Bun);
	Btn_Emotion2->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion2Bun);
	Btn_Emotion3->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion3Bun);
	Btn_Emotion4->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion4Bun);
}

void UHM_EmojiWidget::OnClickedEmotion1Bun()
{
	// 이모션1 애니메이션 재생
	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion1")));
}

void UHM_EmojiWidget::OnClickedEmotion2Bun()
{
	// 이모션2 애니메이션 재생
	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion2")));
}

void UHM_EmojiWidget::OnClickedEmotion3Bun()
{
	// 이모션3 애니메이션 재생
	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion3")));
}

void UHM_EmojiWidget::OnClickedEmotion4Bun()
{
	// 이모션4 애니메이션 재생
	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion4")));
}
