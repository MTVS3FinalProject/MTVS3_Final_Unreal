// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TicketCustom.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_TicketCustom : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_Sticker01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_Sticker02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_Sticker03;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_Sticker04;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_Sticker05;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_TicketBackground;

protected:
	void SetupDraggableImage(UImage* Image);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void SetRotation(FWidgetTransform NewRotation);
	void SetScale(FVector2D NewScale);

	bool bIsDragging;
	FVector2D DragOffset;
    class UImage* CurrentImage;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetSticker01;
	UFUNCTION()
	void OnClickedResetSticker01Button();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetSticker02;
	UFUNCTION()
	void OnClickedResetSticker02Button();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetBackground;
	UFUNCTION()
	void OnClickedResetBackgroundButton();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetTicketImage;
	UFUNCTION()
	void OnClickedResetTicketImageButton();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Save;
	UFUNCTION()
	void OnClickedSaveButton();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;
	UFUNCTION()
	void OnClickedExitButton();
	
};
