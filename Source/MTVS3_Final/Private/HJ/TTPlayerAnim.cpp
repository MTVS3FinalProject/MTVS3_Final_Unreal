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
	Montage_Play(SitDownMontage);
}
