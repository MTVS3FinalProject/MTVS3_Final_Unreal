// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MinimapWidget.h"

#include "PaperSpriteComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "JMH/MH_MinimapActor.h"
#include "Kismet/GameplayStatics.h"

void UHM_MinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	

}

void UHM_MinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);
	
	UpdateMinimapWidget();
}

void UHM_MinimapWidget::UpdateMinimapWidget()
{
	// 플레이어 위치 가져오기
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	// 플레이어 위치와 카메라 방향 가져오기
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FRotator PlayerRotation = PlayerPawn->GetController()->GetControlRotation();

	// Img_StyleLounge의 월드 좌표
	FVector StyleLoungeWorldLocation(18000.0f, 4900.0f, 3300.0f);

	// WBP_MiniMap의 오프셋 및 스케일 계산
	FVector2D MinimapOffset(-170.0f, -170.0f); // 부모 위젯의 설정된 오프셋
	FVector2D MinimapSize(280.0f, 280.0f); // 미니맵 크기
	FVector2D MinimapCenter(MinimapSize * 0.5f); // 미니맵 중심
	//float WorldToMinimapScale = 0.1f; // 월드 -> 미니맵 스케일
	float WorldToMinimapScale = MinimapSize.X / 1505;

	// 월드 좌표를 플레이어 로컬 좌표로 변환
	FVector DeltaLocation = StyleLoungeWorldLocation - PlayerLocation;

	// 플레이어의 Yaw 회전 적용 (월드 좌표를 로컬 미니맵 좌표로 변환)
	float SinYaw, CosYaw;
	FMath::SinCos(&SinYaw, &CosYaw, FMath::DegreesToRadians(PlayerRotation.Yaw));

	FVector2D RotatedLocation(
		DeltaLocation.X * CosYaw - DeltaLocation.Y * SinYaw,
		DeltaLocation.X * SinYaw + DeltaLocation.Y * CosYaw
	);

	// 로컬 좌표를 미니맵 좌표로 변환
	FVector2D StyleLoungeMinimapLocation = RotatedLocation * WorldToMinimapScale;

	// 미니맵 위젯 내 최종 좌표 계산
	FVector2D FinalWidgetPosition = MinimapCenter + StyleLoungeMinimapLocation + MinimapOffset;

	// 좌표를 정수로 반올림하여 UI 움직임 최소화
        FVector2D RoundedPosition(
            FMath::RoundToFloat(FinalWidgetPosition.X),
            FMath::RoundToFloat(FinalWidgetPosition.Y));
	
	// Img_StyleLounge 위치를 업데이트
	if (Img_StyleLounge)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Img_StyleLounge->Slot);
		if (CanvasSlot)
		{
			CanvasSlot->SetPosition(RoundedPosition);
		}
	}
}
