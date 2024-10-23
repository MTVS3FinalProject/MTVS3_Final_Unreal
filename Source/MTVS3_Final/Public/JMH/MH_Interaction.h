// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Interaction.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_Interaction : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_InteractionWidgetSwitcher;
	
	void SetVisibilityInteraction(bool bISVisible, int index);//0: Chair 
};
