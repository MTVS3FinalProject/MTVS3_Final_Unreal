// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_StartWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UButton*  Btn_Exit;
	
};
