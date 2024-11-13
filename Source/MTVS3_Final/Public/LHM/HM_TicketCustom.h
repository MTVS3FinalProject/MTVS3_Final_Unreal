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

	UImage* CopiedImage;
	UImage* Outline;
	UImage* RenderAngle;
	UImage* RenderScale;
	UImage* Delete;
    class UOverlay* ImageGroupOverlay; // 그룹화된 이미지 컨테이너
    UImage* OriginImage; // 원본 이미지 참조 추가

	// 기본 생성자
	FUsedImage() : CopiedImage(nullptr), Outline(nullptr), RenderAngle(nullptr), RenderScale(nullptr), Delete(nullptr), ImageGroupOverlay(nullptr), OriginImage(nullptr) {}
	
	// 파라미터가 있는 생성자
	FUsedImage(UImage* InCopiedImage, UImage* InOutline, UImage* InRenderAngle, UImage* InRenderScale, UImage* InDelete, UOverlay* InOverlay, UImage* InOriginImage)
		: CopiedImage(InCopiedImage), Outline(InOutline), RenderAngle(InRenderAngle), RenderScale(InRenderScale), Delete(InDelete), ImageGroupOverlay(InOverlay), OriginImage(InOriginImage) {}
};
#pragma endregion

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedTicketCustomBack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedTicketCustomSave);

UCLASS()
class MTVS3_FINAL_API UHM_TicketCustom : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnClickedTicketCustomBack OnClickedTicketCustomBack;
	FOnClickedTicketCustomSave OnClickedTicketCustomSave;

	// 닫기 버튼 클릭 이벤트 설정
	UFUNCTION()
	void OnClickedExitButton()
	{
		// 델리게이트 호출
		OnClickedTicketCustomBack.Broadcast();
	}
	
	// 저장 버튼 클릭 이벤트 설정
	UFUNCTION()
	void OnClickedSaveButton()
	{
		// 델리게이트 호출
		OnClickedTicketCustomSave.Broadcast();
	}
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
#pragma region Origin Variable & Custom Properties
	UPROPERTY()
	class UCanvasPanel* RootCanvas;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketBackground;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketInfo;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	TArray<FUsedImage> Img_CopiedImgs;
	
	// 드래그 앤 드롭, 회전, 크기조정, 삭제
	UPROPERTY()
	class UImage* CurrentImage;
	UPROPERTY()
	class UImage* PreviousImage;

	bool bIsDragging;
	bool bIsRenderingAngle;
	bool bIsRenderingScale;
	bool bIsDelete;
	bool bIsBackground;
	FVector2D PreviousMousePosition;
	FVector2D CurrentImageScale;
	
	//void SetupDraggableImage(UImage* Image);
	FUsedImage CreateCompleteImageSet(UImage* SourceImage);
	void SetRenderScale(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	void SetRenderAngle(FUsedImage& ImageSet, const FVector2D& MouseDelta);
	//void DeleteImage(FUsedImage& ImageSet);
	void DeleteImage(FUsedImage& ImageSet, int32 Index);
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	UFUNCTION()
	void SetBackgroundImg(UTexture2D* newTexture);
	
// ================================== 통신에서 받아온 스티커 동적 생성 및 저장 ==================================

	// 동적으로 스티커 이미지를 설정하는 함수
	UFUNCTION()
	void SetStickersImgs(UTexture2D* Texture , int32 ImageIndex);

	// 스티커 이미지 위젯을 초기화하는 함수
	UFUNCTION()
	void InitializeStickerImages(int32 Count);

	// 스크롤박스와 버티컬박스 참조
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox_Stickers;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VerticalBox_Stickers;

	// 동적 이미지 배열
	UPROPERTY()
	TArray<UImage*> StickerImages;

	// 이미지 생성 헬퍼 함수
	class UImage* CreateImageWidget(int32 Index);
    
	// 스페이서 생성 헬퍼 함수
	class USpacer* CreateSpacerWidget();
	
#pragma endregion
	
public:
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
	//UFUNCTION()
	//void OnClickedSaveButtonDELEGATE();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_HttpTest01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_HttpTest02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_HttpTest04;
	UFUNCTION()
	void OnClickedHttpTest01();
	UFUNCTION()
	void OnClickedHttpTest02();
	UFUNCTION()
	void OnClickedHttpTest04();
#pragma endregion
};
