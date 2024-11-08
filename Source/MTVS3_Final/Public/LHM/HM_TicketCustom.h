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
#pragma region Copied Image Set Structs
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
    class UImage* OriginImage; // 원본 이미지 참조 추가

	// 기본 생성자
	FUsedImage() : CopiedImage(nullptr), Outline(nullptr), RenderAngle(nullptr), RenderScale(nullptr), Delete(nullptr), ImageGroupOverlay(nullptr), OriginImage(nullptr) {}
	
	// 파라미터가 있는 생성자
	FUsedImage(UImage* InCopiedImage, UImage* InOutline, UImage* InRenderAngle, UImage* InRenderScale, UImage* InDelete, UOverlay* InOverlay, UImage* InOriginImage)
		: CopiedImage(InCopiedImage), Outline(InOutline), RenderAngle(InRenderAngle), RenderScale(InRenderScale), Delete(InDelete), ImageGroupOverlay(InOverlay), OriginImage(InOriginImage) {}
};
#pragma endregion

UCLASS()
class MTVS3_FINAL_API UHM_TicketCustom : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UCanvasPanel* RootCanvas;

#pragma region Origin Variable
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
#pragma endregion

#pragma region Custom Properties
	// 드래그 앤 드롭, 회전, 크기조정, 삭제
	UPROPERTY()
	class UImage* CurrentImage;

	bool bIsDragging;
	bool bIsRenderingAngle;
	bool bIsRenderingScale;
	bool bIsDelete;
	bool bIsBackground;
	FVector2D PreviousMousePosition;
	FVector2D CurrentImageScale;
	
	void SetupDraggableImage(UImage* Image);
	FUsedImage CreateCompleteImageSet(UImage* SourceImage);
	void SetRenderScale(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	void SetRenderAngle(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	//void DeleteImage(FUsedImage& ImageSet);
	void DeleteImage(FUsedImage& ImageSet, int32 Index);
	void AddImageSetToFinalTicketUI(const FUsedImage& ImageSet);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
#pragma endregion
	
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

protected:
#pragma region FinalTicket Save

	// 최종 이미지 위젯 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Final Display")
	TSubclassOf<class UHM_FinalTicket> FinalTicketWidget;
	
	class UHM_FinalTicket* FinalTicketUI; // 티켓 위젯 참조

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticket Background")
	TSubclassOf<class UHM_TicketBG> TicketBGWidget; // 배경 티켓 위젯 클래스 참조

	class UHM_TicketBG* TicketBGUI; // 배경 티켓 위젯 인스턴스
	
#pragma endregion
};
