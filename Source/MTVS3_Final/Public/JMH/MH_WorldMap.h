// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_WorldMap.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_WorldMap : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	//플레이어 위치 포인터
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_WorldPosition;

	UFUNCTION()
	FVector2D ConvertWorldToMapCoordinates(FVector PlayerLocation, FVector2D WorldMinBounds, FVector2D WorldMaxBounds, FVector2D MapSize);

	UFUNCTION()
	void UpdatePlayerPointer(FVector PlayerLocation, FVector2D WorldMinBounds, FVector2D WorldMaxBounds, FVector2D MapSize);

	UFUNCTION()
	void UpdateWorldMap();
	
};
