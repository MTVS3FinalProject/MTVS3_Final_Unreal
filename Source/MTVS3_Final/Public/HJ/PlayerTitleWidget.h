// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTitleWidget.generated.h"

/**
 * 
 */

UCLASS()
class MTVS3_FINAL_API UPlayerTitleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , meta = (BindWidget))
	class UTextBlock* Tex_TitleName;
	
	void UpdateTitleNameUI(const FString& TitleName);

	void ChangeColorTitleNameUI(const FString& _TitleRarity);
};
