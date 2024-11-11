// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_EmojiWidget.h"
#include "Components/Button.h"
#include "Components/WidgetComponent.h"
#include "HJ/TTPlayer.h"
#include "JMH/MH_EmojiImg.h"

void UHM_EmojiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Emotion1->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion1Bun);
	Btn_Emotion2->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion2Bun);
	Btn_Emotion3->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion3Bun);
	Btn_Emotion4->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion4Bun);
	Btn_Emotion5->OnClicked.AddDynamic(this , &UHM_EmojiWidget::OnClickedEmotion5Bun);
}

void UHM_EmojiWidget::OnClickedEmotion1Bun()
{
	if (!bIsEmojiAnimating)
	{
		// 이모션1 애니메이션 재생
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion1")));
		APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
		ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
		if (Player && Player->EmojiComp)
		{
			// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
			UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
			if (EmojiWidget)
			{
				EmojiWidget->ShowCanvas();
				EmojiWidget->AnimMaterialSwitcher(1);
				// PlayAnim 함수 호출
				EmojiWidget->PlayAnim();
				bIsEmojiAnimating = true;

				//타이머 1.5초 후 HideEmojiWidget
				FTimerHandle RouletteTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UHM_EmojiWidget::HideEmojiWidget ,
				                                       1.5f , false);
			}
		}
	}
}

void UHM_EmojiWidget::OnClickedEmotion2Bun()
{
	if (!bIsEmojiAnimating)
	{
		// 이모션2 애니메이션 재생
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion2")));

		APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
		ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
		if (Player && Player->EmojiComp)
		{
			// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
			UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
			if (EmojiWidget)
			{
				EmojiWidget->ShowCanvas();
				EmojiWidget->AnimMaterialSwitcher(2);
				// PlayAnim 함수 호출
				EmojiWidget->PlayAnim();
				bIsEmojiAnimating = true;
				//타이머 1.5초 후 HideEmojiWidget
				FTimerHandle RouletteTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UHM_EmojiWidget::HideEmojiWidget ,
				                                       1.5f , false);
			}
		}
	}
}

void UHM_EmojiWidget::OnClickedEmotion3Bun()
{
	if (!bIsEmojiAnimating)
	{
		// 이모션3 애니메이션 재생
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion3")));

		APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
		ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
		if (Player && Player->EmojiComp)
		{
			// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
			UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
			if (EmojiWidget)
			{
				EmojiWidget->ShowCanvas();
				EmojiWidget->AnimMaterialSwitcher(3);
				// PlayAnim 함수 호출
				EmojiWidget->PlayAnim();
				bIsEmojiAnimating = true;
				//타이머 1.5초 후 HideEmojiWidget
				FTimerHandle RouletteTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UHM_EmojiWidget::HideEmojiWidget ,
				                                       1.5f , false);
			}
		}
	}
}

void UHM_EmojiWidget::OnClickedEmotion4Bun()
{
	if (!bIsEmojiAnimating)
	{
		// 이모션4 애니메이션 재생
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion4")));
		APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
		ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
		if (Player && Player->EmojiComp)
		{
			// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
			UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
			if (EmojiWidget)
			{
				EmojiWidget->ShowCanvas();
				EmojiWidget->AnimMaterialSwitcher(4);
				// PlayAnim 함수 호출
				EmojiWidget->PlayAnim();
				bIsEmojiAnimating = true;
				//타이머 1.5초 후 HideEmojiWidget
				FTimerHandle RouletteTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UHM_EmojiWidget::HideEmojiWidget ,
													   1.5f , false);
			}
		}
	}
}

void UHM_EmojiWidget::OnClickedEmotion5Bun()
{
	if (!bIsEmojiAnimating)
	{
		// 이모션4 애니메이션 재생
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Play Anim Emotion5")));
		APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
		ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
		if (Player && Player->EmojiComp)
		{
			// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
			UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
			if (EmojiWidget)
			{
				EmojiWidget->ShowCanvas();
				EmojiWidget->AnimMaterialSwitcher(5);
				// PlayAnim 함수 호출
				EmojiWidget->PlayAnim();
				bIsEmojiAnimating = true;
				//타이머 1.5초 후 HideEmojiWidget
				FTimerHandle RouletteTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UHM_EmojiWidget::HideEmojiWidget ,
													   1.5f , false);
			}
		}
	}
}

void UHM_EmojiWidget::HideEmojiWidget()
{
	APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
	ATTPlayer* Player = Cast<ATTPlayer>(PC->GetCharacter());
	if (Player && Player->EmojiComp)
	{
		// UWidgetComponent로부터 Emoji 위젯 인스턴스 가져오기
		UMH_EmojiImg* EmojiWidget = Cast<UMH_EmojiImg>(Player->EmojiComp->GetUserWidgetObject());
		if (EmojiWidget)
		{
			if (bIsEmojiAnimating)
			{
				EmojiWidget->HideCanvas();
				bIsEmojiAnimating = false;
			}
		}
	}
}
