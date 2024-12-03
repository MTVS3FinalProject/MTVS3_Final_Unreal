// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_ErrorMessage.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_ErrorMessage : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ErrorMessage;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_ErrorExit;
	
};
