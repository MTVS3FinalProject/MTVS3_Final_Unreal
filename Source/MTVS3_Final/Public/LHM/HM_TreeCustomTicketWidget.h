// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TreeCustomTicketWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_TreeCustomTicketWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UHorizontalBox* Hori_Box_Tickets;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UHM_TreeTicketBoxWidget> TicketItemBoxFac;

	UFUNCTION()
	void InitializeTicketTabs(const TMap<int32, FString>& TicketMap);

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_Choose;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Yes;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_No;
	UFUNCTION()
	void OnClickedYesBtn();
	UFUNCTION()
	void OnClickedNoBtn();
	void SetCanChooseVisibility(bool bVisible , int32 SelectedTicketId);
	
private:
	int32 CurrentSelectedTicketId;
};
