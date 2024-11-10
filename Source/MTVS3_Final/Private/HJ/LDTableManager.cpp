// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LDTableManager.h"

// Sets default values
ALDTableManager::ALDTableManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALDTableManager::BeginPlay()
{
	Super::BeginPlay();

	// LuckyDrawTable 컴포넌트를 배열에 저장
	TArray<UChildActorComponent*> LuckyDrawTableComponents;
	GetComponents(LuckyDrawTableComponents);

	// 테이블 컴포넌트 위치에 따른 태그 번호 부여를 위한 정렬 (10행 3열)
	LuckyDrawTableComponents.Sort([](const UChildActorComponent& A, const UChildActorComponent& B) {
		FVector LocationA = A.GetComponentLocation();
		FVector LocationB = B.GetComponentLocation();

		// X 값이 작을수록 앞열로 배치, 같은 X 값에서는 Y 값이 클수록 아래쪽 배치
		if (FMath::Abs(LocationA.Y - LocationB.Y) > KINDA_SMALL_NUMBER)
		{
			return LocationA.Y > LocationB.Y; // Y 값이 큰 것이 윗 행(스크린에 보이는 기준으로 설정)
		}
		return LocationA.X < LocationB.X;     // 같은 Y 값이면 X 값으로 비교
	});

	// 정렬된 컴포넌트들에 TableTag 부여 (10행 3열 기준)
	for (int32 Index = 0; Index < LuckyDrawTableComponents.Num(); ++Index)
	{
		UChildActorComponent* ChildComponent = LuckyDrawTableComponents[Index];
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			// 태그를 "Table_1", "Table_2", ..., "Table_30" 형식으로 추가
			FString Tag = FString::Printf(TEXT("Table_%d"), Index + 1);
			ChildComponent->ComponentTags.Add(FName(*Tag));

			UE_LOG(LogTemp, Log, TEXT("Added Tag: %s to Table Component at index %d"), *Tag, Index + 1);
		}
	}
}

// Called every frame
void ALDTableManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

