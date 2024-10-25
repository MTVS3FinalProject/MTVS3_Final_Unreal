// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/PlayerNicknameWidget.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UPlayerNicknameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 게임 인스턴스를 가져와서
	auto* GI = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if ( !GI || !Local || !PS ) return;

	// 기본 색상 설정
	FColor RoleColor = FColor::White;

	switch ( GI->GetPlaceState() )
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		Tex_Nickname->SetText(FText::FromString(PS->GetNickname()));
		break;
	case EPlaceState::LuckyDrawRoom:
		Tex_Nickname->SetText(FText::FromString(FString::FromInt(PS->GetRandomSeatNumber())));
		break;
	}

	// 닉네임 색상 설정
	Tex_Nickname->SetColorAndOpacity(FSlateColor(RoleColor));
}