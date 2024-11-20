// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawLine.h"

#include "Components/BoxComponent.h"

// Sets default values
ALuckyDrawLine::ALuckyDrawLine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);
	PrimaryActorTick.bCanEverTick = true;

	// 배열 초기 크기 설정
	VerticalBoxes.SetNum(10);
	HorizontalBoxes.SetNum(3);

	// 세로 열 박스 생성 (Box1 ~ Box10)
	for (int32 i = 0; i < 10; i++)
	{
		FString BoxName = FString::Printf(TEXT("Box%d"), i + 1);
		VerticalBoxes[i] = CreateDefaultSubobject<UBoxComponent>(*BoxName);
		VerticalBoxes[i]->SetupAttachment(RootComponent);
		VerticalBoxes[i]->SetBoxExtent(FVector(300.0f, 32.0f, 32.0f));
		VerticalBoxes[i]->SetRelativeLocation(FVector(0.0f, -133.0f * i, 0.0f));
		VerticalBoxes[i]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		VerticalBoxes[i]->SetCollisionProfileName(TEXT("NoCollision"));
	}

	// 가로 행 박스 생성 (Box11 ~ Box13)
	for (int32 i = 0; i < 3; i++)
	{
		FString BoxName = FString::Printf(TEXT("Box%d"), i + 11);
		HorizontalBoxes[i] = CreateDefaultSubobject<UBoxComponent>(*BoxName);
		HorizontalBoxes[i]->SetupAttachment(RootComponent);
		HorizontalBoxes[i]->SetBoxExtent(FVector(700.0f, 32.0f, 32.0f));
        
		// Box13(i=2)일 때는 -215, Box12(i=1)일 때는 215, Box11(i=0)일 때는 0
		float XOffset = (i == 0) ? 0.0f : ((i == 1) ? 215.0f : -215.0f);
        
		HorizontalBoxes[i]->SetRelativeLocation(FVector(XOffset, -605.0f, 0.0f));
		HorizontalBoxes[i]->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		HorizontalBoxes[i]->SetCollisionProfileName(TEXT("NoCollision"));
	}
}

// Called when the game starts or when spawned
void ALuckyDrawLine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALuckyDrawLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALuckyDrawLine::ActivateLinesByRule(ERouletteRule Rule, int32 SelectedRow, int32 SelectedCol)
{
	// 배열 유효성 검사
	if (VerticalBoxes.Num() == 0 || HorizontalBoxes.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("LuckyDrawLine: Box arrays are empty"));
		return;
	}

	// 모든 라인을 먼저 비활성화
	DeactivateAllLines();

	switch (Rule)
	{
	case ERouletteRule::SameColumnOnly:
		// 선택된 열의 수직 박스만 활성화
			if (SelectedCol >= 0 && SelectedCol < VerticalBoxes.Num() && VerticalBoxes[SelectedCol] != nullptr)
			{
				VerticalBoxes[SelectedCol]->SetCollisionProfileName(TEXT("OverlapAll"));
				UE_LOG(LogTemp, Log, TEXT("LuckyDrawLine: Activating vertical box %d"), SelectedCol);
			}
		break;

	case ERouletteRule::SameRowOnly:
		// 선택된 행의 수평 박스만 활성화
			if (SelectedRow >= 0 && SelectedRow < HorizontalBoxes.Num() && HorizontalBoxes[SelectedRow] != nullptr)
			{
				HorizontalBoxes[SelectedRow]->SetCollisionProfileName(TEXT("OverlapAll"));
				UE_LOG(LogTemp, Log, TEXT("LuckyDrawLine: Activating horizontal box %d"), SelectedRow);
			}
		break;

	case ERouletteRule::ExcludeSameColumn:
		// 선택된 열을 제외한 모든 수직 박스 활성화
			for (int32 i = 0; i < VerticalBoxes.Num(); ++i)
			{
				if (i != SelectedCol && VerticalBoxes[i] != nullptr)
				{
					VerticalBoxes[i]->SetCollisionProfileName(TEXT("OverlapAll"));
				}
			}
		break;

	case ERouletteRule::ExcludeSameRow:
		// 선택된 행을 제외한 모든 수평 박스 활성화
			for (int32 i = 0; i < HorizontalBoxes.Num(); ++i)
			{
				if (i != SelectedRow && HorizontalBoxes[i] != nullptr)
				{
					HorizontalBoxes[i]->SetCollisionProfileName(TEXT("OverlapAll"));
				}
			}
		break;

	default:
		break;
	}
}

void ALuckyDrawLine::DeactivateAllLines()
{
	// 모든 수직 박스 비활성화
	for (UBoxComponent* Box : VerticalBoxes)
	{
		if (Box != nullptr)
		{
			Box->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}

	// 모든 수평 박스 비활성화
	for (UBoxComponent* Box : HorizontalBoxes)
	{
		if (Box != nullptr)
		{
			Box->SetCollisionProfileName(TEXT("NoCollision"));
		}
	}
}

TArray<ALuckyDrawChair*> ALuckyDrawLine::GetOverlappingChairs()
{
	TArray<ALuckyDrawChair*> OverlappingChairs;

    // 월드 가져오기
    UWorld* World = GetWorld();
    if (!World) return OverlappingChairs;

    // 각 활성화된 박스에 대해 오버랩 체크 수행
    for (UBoxComponent* Box : VerticalBoxes)
    {
        if (Box && Box->GetCollisionProfileName() == TEXT("OverlapAll"))
        {
            FVector BoxLocation = Box->GetComponentLocation();
            FRotator BoxRotation = Box->GetComponentRotation();
            FVector BoxExtent = Box->GetScaledBoxExtent();

            TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams;
            QueryParams.bTraceComplex = false;
            QueryParams.AddIgnoredActors(IgnoreActors);

            // 박스 영역 내의 모든 오브젝트 검출
            bool bOverlapped = World->OverlapMultiByObjectType(
                Overlaps,
                BoxLocation,
                BoxRotation.Quaternion(),
                FCollisionObjectQueryParams::InitType::AllObjects,
                FCollisionShape::MakeBox(BoxExtent),
                QueryParams
            );

            if (bOverlapped)
            {
                for (const FOverlapResult& Overlap : Overlaps)
                {
                    if (ALuckyDrawChair* Chair = Cast<ALuckyDrawChair>(Overlap.GetActor()))
                    {
                        OverlappingChairs.AddUnique(Chair);
                    }
                }
            }
        }
    }

    // 수평 박스에 대해서도 동일한 체크 수행
    for (UBoxComponent* Box : HorizontalBoxes)
    {
        if (Box && Box->GetCollisionProfileName() == TEXT("OverlapAll"))
        {
            FVector BoxLocation = Box->GetComponentLocation();
            FRotator BoxRotation = Box->GetComponentRotation();
            FVector BoxExtent = Box->GetScaledBoxExtent();

            TArray<FOverlapResult> Overlaps;
            FCollisionQueryParams QueryParams;
            QueryParams.bTraceComplex = false;
            QueryParams.AddIgnoredActors(IgnoreActors);

            bool bOverlapped = World->OverlapMultiByObjectType(
                Overlaps,
                BoxLocation,
                BoxRotation.Quaternion(),
                FCollisionObjectQueryParams::InitType::AllObjects,
                FCollisionShape::MakeBox(BoxExtent),
                QueryParams
            );

            if (bOverlapped)
            {
                for (const FOverlapResult& Overlap : Overlaps)
                {
                    if (ALuckyDrawChair* Chair = Cast<ALuckyDrawChair>(Overlap.GetActor()))
                    {
                        OverlappingChairs.AddUnique(Chair);
                    }
                }
            }
        }
    }

    // 디버그 로그
    UE_LOG(LogTemp, Warning, TEXT("Found %d overlapping chairs"), OverlappingChairs.Num());
    for (ALuckyDrawChair* Chair : OverlappingChairs)
    {
        UE_LOG(LogTemp, Log, TEXT("Found overlapping chair: %s"), *Chair->GetName());
    }

    return OverlappingChairs;
}

void ALuckyDrawLine::GetChairsOverlappingBox(UBoxComponent* Box, TArray<ALuckyDrawChair*>& OutChairs)
{
	TArray<AActor*> OverlappingActors;
	Box->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (ALuckyDrawChair* Chair = Cast<ALuckyDrawChair>(Actor))
		{
			OutChairs.AddUnique(Chair);
		}
	}
}
