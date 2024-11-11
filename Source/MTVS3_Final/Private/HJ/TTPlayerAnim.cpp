// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayerAnim.h"
#include "HJ/TTPlayer.h"

void UTTPlayerAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Me = Cast<ATTPlayer>(TryGetPawnOwner());
}

void UTTPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if ( nullptr == Me ) return;

}

void UTTPlayerAnim::PlaySitDownMontage()
{
	if ( Me->bIsSitting && SitDownMontage )
	{
		Montage_Play(SitDownMontage);
	}
}

void UTTPlayerAnim::PlayStandUpMontage()
{
	if ( !Me->bIsSitting && StandUpMontage )
	{
		Montage_Play(StandUpMontage);
	}
}

void UTTPlayerAnim::PlaySittingMontage()
{
	if (SitDownMontage) Montage_Play(SitDownMontage);
}

void UTTPlayerAnim::PlayDancingMontage()
{
	if (DancingMontage) Montage_Play(DancingMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f);
}

void UTTPlayerAnim::PlayEmojiMontage(const int32& EmojiNum)
{
	switch(EmojiNum)
	{
	case 1:
		if (Emoji1Montage) Montage_Play(Emoji1Montage);
		break;
	case 2:
		if (Emoji2Montage) Montage_Play(Emoji2Montage);
		break;
	case 3:
		if (Emoji3Montage) Montage_Play(Emoji3Montage);
		break;
	case 4:
		if (Emoji4Montage) Montage_Play(Emoji4Montage);
		break;
	case 5:
		if (Emoji5Montage) Montage_Play(Emoji5Montage);
		break;
	default:
		if (Emoji1Montage) Montage_Play(Emoji1Montage);
		break;
	}
}
