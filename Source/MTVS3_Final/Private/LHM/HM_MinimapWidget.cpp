// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MinimapWidget.h"

#include "PaperSpriteComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_MinimapActor.h"
#include "Kismet/GameplayStatics.h"

void UHM_MinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	

}

void UHM_MinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	UpdateTargetPosition(StyleLounge , Img_StyleLounge);
	UpdateTargetPosition(Community , Img_Community);
	UpdateTargetPosition(Kiosk_1 , Img_Kiosk_1);
	UpdateTargetPosition(Kiosk_2 , Img_Kiosk_2);
	UpdateTargetPosition(Kiosk_3 , Img_Kiosk_3);
	UpdateTargetPosition(Kiosk_4 , Img_Kiosk_4);
	
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
		SetImgsVisible(true);
		break;
	case EPlaceState::StyleLounge:
		SetImgsVisible(true);
		break;
	case EPlaceState::CommunityHall:
		SetImgsVisible(true);
		break;
	case EPlaceState::ConcertHall:
		SetImgsVisible(false);
		break;
	}
}

void UHM_MinimapWidget::SetImgsVisible(bool visible)
{
	if(visible)
	{
		Img_StyleLounge->SetVisibility(ESlateVisibility::Visible);
		Img_Community->SetVisibility(ESlateVisibility::Visible);
		Img_Kiosk_1->SetVisibility(ESlateVisibility::Visible);
		Img_Kiosk_2->SetVisibility(ESlateVisibility::Visible);
		Img_Kiosk_3->SetVisibility(ESlateVisibility::Visible);
		Img_Kiosk_4->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Img_StyleLounge->SetVisibility(ESlateVisibility::Hidden);
		Img_Community->SetVisibility(ESlateVisibility::Hidden);
		Img_Kiosk_1->SetVisibility(ESlateVisibility::Hidden);
		Img_Kiosk_2->SetVisibility(ESlateVisibility::Hidden);
		Img_Kiosk_3->SetVisibility(ESlateVisibility::Hidden);
		Img_Kiosk_4->SetVisibility(ESlateVisibility::Hidden);	
	}
}

void UHM_MinimapWidget::UpdateTargetPosition(FVector TargetLocation, UImage* Img_Target)
{
	// 플레이어 위치 가져오기
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FRotator PlayerRotation = PlayerPawn->GetController()->GetControlRotation();

	FVector DeltaLocation = TargetLocation - PlayerLocation;

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

	// 미니맵 원형 반경 계산
	float MinimapRadius = MapSize.X * 0.5f; // 원의 반경
	FVector2D CircleCenter = MinimapCenter + MinimapOffset; // 원의 중심

	// 원의 반경을 초과하는지 확인
	float DistanceFromCenter = (FinalWidgetPosition - CircleCenter).Size();
	if (DistanceFromCenter > MinimapRadius)
	{
		// 원 경계로 위치 제한
		FVector2D Direction = (FinalWidgetPosition - CircleCenter).GetSafeNormal();
		FinalWidgetPosition = CircleCenter + Direction * MinimapRadius;
	}

	// UI 위젯 좌표 설정
	if (Img_Target)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Img_Target->Slot);
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
