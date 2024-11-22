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
#pragma region Sticker Data
USTRUCT(BlueprintType)
struct FStickerData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 StickerId;

	UPROPERTY()
	UTexture2D* StickerImage;

	// 기본 생성자
	FStickerData() : StickerId(-1), StickerImage(nullptr) {}
	FStickerData(int32 InStickerId, UTexture2D* InStickerImage)
		: StickerId(InStickerId), StickerImage(InStickerImage) {}
};
#pragma endregion

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
	FVector2D CurrentScale; // 현재 Scale 값을 저장

	// 기본 생성자
	FUsedImage() : CopiedImage(nullptr), Outline(nullptr), RenderAngle(nullptr), RenderScale(nullptr), Delete(nullptr), ImageGroupOverlay(nullptr), OriginImage(nullptr), CurrentScale(1) {}
	
	// 파라미터가 있는 생성자
	FUsedImage(UImage* InCopiedImage, UImage* InOutline, UImage* InRenderAngle, UImage* InRenderScale, UImage* InDelete, UOverlay* InOverlay, UImage* InOriginImage, FVector2D InCurrentScale)
		: CopiedImage(InCopiedImage), Outline(InOutline), RenderAngle(InRenderAngle), RenderScale(InRenderScale), Delete(InDelete), ImageGroupOverlay(InOverlay), OriginImage(InOriginImage), CurrentScale(InCurrentScale) {}
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
	
	// // 저장 버튼 클릭 이벤트 설정
	// UFUNCTION()
	// void OnClickedSaveButton()
	// {
	// 	// 델리게이트 호출
	// 	OnClickedTicketCustomSave.Broadcast();
	// }
	UFUNCTION()
	void OnClickedSaveButton();
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
#pragma region Origin Variable & Custom Properties
	UPROPERTY()
	class UCanvasPanel* RootCanvas;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketBackground;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_TicketInfo;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_ConcertName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Year;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Day;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Month;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Time;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_SeatInfo;

	UFUNCTION()
	void SetTextTicketInfo(FString _ConcertName, int32 _Year, int32 _Monthm, int32 _Day, FString _Time, FString _SeatInfo);
	
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

	//============================================ 스티커 이미지와 아이디 배열 테스트
	// 동적 StickerImages와 StickerId 배열
	UPROPERTY()
	TArray<FStickerData> StickerDataArray;

	UFUNCTION()
	void AddSticker(int32 StickerId, UTexture2D* StickerImage);
	
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
	
#pragma endregion
};
