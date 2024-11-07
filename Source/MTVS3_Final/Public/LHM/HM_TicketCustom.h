// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "HM_TicketCustom.generated.h"

class UCanvasPanelSlot;
/**
 * 
 */
USTRUCT()
struct FUsedImage
{
	GENERATED_BODY()

	class UImage* CopiedImage;
	class UImage* Outline;
	class UImage* RenderAngle;
	class UImage* RenderScale;
	class UImage* Delete;
    class UOverlay* ImageGroupOverlay; // 그룹화된 이미지 컨테이너

	// 기본 생성자
	FUsedImage() : CopiedImage(nullptr), Outline(nullptr), RenderAngle(nullptr), RenderScale(nullptr), Delete(nullptr), ImageGroupOverlay(nullptr) {}
	
	// 파라미터가 있는 생성자
	FUsedImage(UImage* InCopiedImage, UImage* InOutline, UImage* InRenderAngle, UImage* InRenderScale, UImage* InDelete, UOverlay* InOverlay)
		: CopiedImage(InCopiedImage), Outline(InOutline), RenderAngle(InRenderAngle), RenderScale(InRenderScale), Delete(InDelete), ImageGroupOverlay(InOverlay) {}
};

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
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TArray<FUsedImage> Img_CopiedImgs;

	bool bIsDragging;
	FVector2D DragOffset;
	FVector2D OriginalPosition;
	UPROPERTY()
    class UImage* CurrentImage;

	bool bIsRenderingAngle;
	bool bIsRenderingScale;
	bool bIsDelete;
	FVector2D PreviousMousePosition;
	
	void SetupDraggableImage(UImage* Image);
	FUsedImage CreateCompleteImageSet(UImage* SourceImage);
	void SetRenderScale(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	void SetRenderAngle(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	void DeleteImage(FUsedImage& ImageSet);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;


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
