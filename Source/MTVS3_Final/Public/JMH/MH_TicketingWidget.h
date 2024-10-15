// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_TicketingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_TicketingWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_RegisterSwitcher;
};
