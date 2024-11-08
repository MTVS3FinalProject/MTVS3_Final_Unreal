// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TicketBG.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_TicketBG : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	class UCanvasPanel* RootCanvas;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketBackground;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketInfo;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay_TicketBG;

#pragma region FinalTicket Save
#pragma endregion

private:
	virtual void NativeConstruct() override;
};
