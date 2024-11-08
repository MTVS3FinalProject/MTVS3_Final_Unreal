// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HM_HTTPActor_Inven.generated.h"

USTRUCT(BlueprintType)
struct FTitleItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 TitleID;

	UPROPERTY(BlueprintReadOnly)
	FString TitleName;

	UPROPERTY(BlueprintReadOnly)
	FString TitleScript;
	
	UPROPERTY(BlueprintReadOnly)
	FString TitleRarity;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEquipped;
};

USTRUCT(BlueprintType)
struct FTicketItemData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 StickerID;

	UPROPERTY(BlueprintReadOnly)
	FString StickerScript;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* StickerImg;
};

USTRUCT(BlueprintType)
struct FStickerItemData
{
	GENERATED_BODY()
		
	UPROPERTY(BlueprintReadOnly)
	FString TicketID;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* TicketImage;
};

UCLASS()
class MTVS3_FINAL_API AHM_HTTPActor_Inven : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_HTTPActor_Inven();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	// 서버로 인벤토리 데이터 요청
	void RequestInventoryData();

	// 인벤토리 데이터를 가져오는 함수
	const TArray<FTitleItemData>& GetTitleItems() const { return TitleItems; }
	const TArray<FTicketItemData>& GetTicketItems() const { return TicketItems; }
	const TArray<FStickerItemData>& GetStickerItems() const { return StickerItems; }

	// 데이터 저장용 TArray
	TArray<FTitleItemData> TitleItems;
	TArray<FTicketItemData> TicketItems;
	TArray<FStickerItemData> StickerItems;
	
	// 서버 응답 처리 함수
	void OnInventoryDataReceived(const FString& JsonResponse);
};
