// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TTPlayerAnim.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API UTTPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class ATTPlayer* Me;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* SitDownMontage;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* StandUpMontage;
	
	void PlaySitDownMontage();

	void PlayStandUpMontage();

	void PlaySittingMontage();

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* DancingMontage;
	
	void PlayDancingMontage();

	// 이모티콘
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Emoji1Montage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Emoji2Montage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Emoji3Montage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Emoji4Montage;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Emoji5Montage;

	void PlayEmojiMontage(const int32& EmojiNum);
};
