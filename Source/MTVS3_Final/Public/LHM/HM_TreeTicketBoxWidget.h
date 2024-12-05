// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TreeTicketBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_TreeTicketBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Ticket;
	UFUNCTION()
	void OnImgTicketClicked();
	
	UPROPERTY(EditAnywhere)
	int32 TicketId;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_Ticket;
};
