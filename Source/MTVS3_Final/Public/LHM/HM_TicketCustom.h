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

	UPROPERTY()
	class UCanvasPanel* RootCanvas;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UImage* Img_TicketBackground;
	
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
	TArray<UImage*> Img_Copieds;
	
	void SetupDraggableImage(UImage* Image);
	UImage* CreateDraggableImageCopy(UImage* SourceImage);
	void SetRenderScale(UImage* Image, const FVector2D& MouseDelta);
	void SetRenderAngle(UImage* Image, const FVector2D& MouseDelta);
	
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
	TArray<UImage*> DraggedImageCopies;


#pragma region Button Functions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetBackground;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_ResetTicketImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Save;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;
	
	UFUNCTION()
	void OnClickedResetBackgroundButton();
	UFUNCTION()
	void OnClickedResetTicketImageButton();
	UFUNCTION()
	void OnClickedSaveButton();
	UFUNCTION()
	void OnClickedExitButton();
#pragma endregion
};
