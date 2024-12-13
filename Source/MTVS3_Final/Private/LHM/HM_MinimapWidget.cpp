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

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FRotator PlayerRotation = PlayerPawn->GetController()->GetControlRotation();

	// Img_StyleLounge 월드 좌표
	FVector StyleLoungeWorldLocation(18000.0f, 4900.0f, 3300.0f);
	FVector DeltaLocation = StyleLoungeWorldLocation - PlayerLocation;

	// 플레이어 방향(Yaw) 적용하여 로컬 좌표 계산
	float SinYaw, CosYaw;
	FMath::SinCos(&SinYaw, &CosYaw, FMath::DegreesToRadians(PlayerRotation.Yaw+90));
	
	FVector2D RotatedLocation(
		DeltaLocation.X * CosYaw + DeltaLocation.Y * SinYaw, // X와 Y축의 변환 보정
		DeltaLocation.X * -SinYaw + DeltaLocation.Y * CosYaw // 뒤집기 보정
	);
	
	// 미니맵 크기 및 스케일 정의
	FVector2D MinimapOffset(-130.0f); // 부모 위젯의 설정된 오프셋
	FVector2D MapSize(256.0f, 256.0f);
	float WorldToMapScale = MapSize.X / 1500; // 미니맵 월드 변환 스케일
	
	FVector2D MinimapPosition = RotatedLocation * WorldToMapScale;

	// 미니맵 중심 오프셋 추가
	FVector2D MinimapCenter(MapSize * 0.5f);
	FVector2D FinalWidgetPosition = MinimapCenter + MinimapPosition + MinimapOffset;

	// UI 위젯 좌표 설정
	if (Img_StyleLounge)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Img_StyleLounge->Slot);
		if (CanvasSlot)
		{
			// 위치를 정수 좌표로 반올림하여 업데이트
			CanvasSlot->SetPosition(FVector2D(
				FMath::RoundToFloat(FinalWidgetPosition.X),
				FMath::RoundToFloat(FinalWidgetPosition.Y)
			));
		}
	}
}