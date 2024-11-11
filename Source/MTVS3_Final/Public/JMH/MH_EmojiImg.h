// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_EmojiImg.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_EmojiImg : public UUserWidget
{
	GENERATED_BODY()

public:

	//Visible 여부 결정할 캔버스
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UCanvasPanel* Can_Emoji;
	
	//이모티콘이 나타날 이미지
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Emoji;
	
	//애니메이션 머티리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="EmojiAnim")
	UMaterialInterface* HappyAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="EmojiAnim")
	UMaterialInterface* KissAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="EmojiAnim")
	UMaterialInterface* LoveAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="EmojiAnim")
	UMaterialInterface* SadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="EmojiAnim")
	UMaterialInterface* SurprisedAnim;

	UFUNCTION()
	void SetMaterial(UMaterialInterface* NewAnim);
	
	UFUNCTION()
	void AnimMaterialSwitcher(int32 num);
	
	UFUNCTION()
	void ShowCanvas();
	UFUNCTION()
	void HideCanvas();

	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* EmojiVisibleAnim;
	
	UFUNCTION(Blueprintable)
	void PlayAnim();
};
