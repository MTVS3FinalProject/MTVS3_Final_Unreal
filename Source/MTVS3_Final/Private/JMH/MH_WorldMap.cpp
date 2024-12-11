// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_WorldMap.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UMH_WorldMap::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateWorldMap();
	SetVisibleSwitcher(false);
}

void UMH_WorldMap::SetVisibleSwitcher(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
		UpdateWorldMap();
	}
	
	else if(!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

FVector2D UMH_WorldMap::ConvertWorldToMapCoordinates(FVector PlayerLocation, FVector2D WorldMinBounds, FVector2D WorldMaxBounds, FVector2D MapSize)
{
	float XRatio = (PlayerLocation.X - WorldMinBounds.X) / (WorldMaxBounds.X - WorldMinBounds.X);  // X 범위를 [0, 1]로 변환
	float YRatio = (PlayerLocation.Y - WorldMinBounds.Y) / (WorldMaxBounds.Y - WorldMinBounds.Y);  // Y 범위를 [0, 1]로 변환

	// Y 축 방향 반전
	YRatio = 1.0f - YRatio;
	
	float MapX = XRatio * MapSize.X;
	float MapY = YRatio * MapSize.Y;
	
	return FVector2D(MapX, MapY);

}

void UMH_WorldMap::UpdatePlayerPointer(FVector PlayerLocation, FVector2D WorldMinBounds, FVector2D WorldMaxBounds, FVector2D MapSize)
{
	FVector2D MapPosition = ConvertWorldToMapCoordinates(PlayerLocation, WorldMinBounds, WorldMaxBounds, MapSize);

	// 변환된 좌표를 로그로 출력
	UE_LOG(LogTemp, Warning, TEXT("Player Pointer Position: %s"), *MapPosition.ToString());
	
	if (Img_WorldPosition)
	{
		// 플레이어 포인터의 위치를 맵 좌표로 설정
		Img_WorldPosition->SetRenderTranslation(MapPosition);
	}
}

//월드맵의 플레이어 위치 업데이트 해주기. 버튼 누를때 마다
void UMH_WorldMap::UpdateWorldMap()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();

		// 월드의 최소 및 최대 좌표 설정 (예시 범위, 필요 시 조정)
		FVector2D WorldMinBounds = FVector2D(-6600.f, -6600.f);  // 월드 좌표 최소값
		FVector2D WorldMaxBounds = FVector2D(6600.f, 6600.f);    // 월드 좌표 최대값
		FVector2D MapSize = FVector2D(900.f, 900.f);             // 맵 크기

		// 플레이어 포인터 위치 업데이트
		UpdatePlayerPointer(PlayerLocation, WorldMinBounds, WorldMaxBounds, MapSize);
		
	}
	
}
