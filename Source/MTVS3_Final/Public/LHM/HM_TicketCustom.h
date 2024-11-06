// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_TicketCustom.generated.h"

class UCanvasPanelSlot;
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetSticker01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetSticker02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetBackground;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetTicketImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Save;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;
	
	UFUNCTION()
	void OnClickedResetSticker01Button();
	UFUNCTION()
	void OnClickedResetSticker02Button();
	UFUNCTION()
	void OnClickedResetBackgroundButton();
	UFUNCTION()
	void OnClickedResetTicketImageButton();
	UFUNCTION()
	void OnClickedSaveButton();
	UFUNCTION()
	void OnClickedExitButton();

	UPROPERTY()
	class UCanvasPanel* RootCanvas;

	
	void SetupDraggableImage(UImage* Image);
	UImage* CreateDraggableImageCopy(UImage* SourceImage);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	bool bIsDragging;
	FVector2D DragOffset;
    class UImage* CurrentImage;
	FVector2D OriginalPosition;
	
	UPROPERTY()
	UImage* DraggedImageCopy;
	UPROPERTY()
	TMap<UImage*, FString> CopiedImages;
	UFUNCTION()
	void OnCopiedImageClicked();
};
