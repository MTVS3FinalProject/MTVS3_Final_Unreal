// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/PlayerNicknameWidget.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayerState.h"

void UPlayerNicknameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//// 게임 인스턴스를 가져와서
	//auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	//if (gi)
	//{
	//	Tex_Nickname->SetText(FText::FromString(gi->GetNickname()));
	//}

	// PS에 닉네임 저장
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if ( PS ) Tex_Nickname->SetText(FText::FromString(PS->GetNickname()));
}
