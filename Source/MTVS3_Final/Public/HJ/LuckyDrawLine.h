// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuckyDrawChair.h"
#include "TTLuckyDrawGameMode.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawLine.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALuckyDrawLine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* SceneComp;
	
	// 세로 열 박스 배열 (Box1 ~ Box10)
	UPROPERTY(EditAnywhere, Category = "Components|Vertical")
	TArray<class UBoxComponent*> VerticalBoxes;
    
	// 가로 행 박스 배열 (Box11 ~ Box13)
	UPROPERTY(EditAnywhere, Category = "Components|Horizontal")
	TArray<class UBoxComponent*> HorizontalBoxes;
	
	// 룰렛 규칙에 따라 라인을 활성화하는 함수
	void ActivateLinesByRule(ERouletteRule Rule, int32 SelectedRow, int32 SelectedCol);
	// 모든 라인의 콜리전을 비활성화하는 함수
	void DeactivateAllLines();
    
	// 오버랩된 의자들을 가져오는 함수
	TArray<ALuckyDrawChair*> GetOverlappingChairs();

	// 오버랩 체크 시 무시할 액터들의 배열
	UPROPERTY()
	TArray<AActor*> IgnoreActors;

private:
	// 특정 박스 컴포넌트와 오버랩된 의자들을 가져오는 함수
	void GetChairsOverlappingBox(UBoxComponent* Box, TArray<ALuckyDrawChair*>& OutChairs);
};
