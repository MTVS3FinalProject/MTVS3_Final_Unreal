// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePiece.h"

#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/PuzzleManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHM_PuzzlePiece::AHM_PuzzlePiece()
 {
  	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    
	PrimaryActorTick.bCanEverTick = true;
    
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    
	// 컴포넌트만 생성하고 기본 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
		TEXT("/Script/Engine.StaticMesh'/Game/KHJ/Assets/SM_BoxBrush.SM_BoxBrush'"));
	if (MeshAsset.Succeeded())
	{
		PieceMeshes.SetNum(9);
		for (int32 i = 0; i < 9; i++)
		{
			FName PieceName = *FString::Printf(TEXT("Piece%d"), i + 1);
			PieceMeshes[i] = CreateDefaultSubobject<UStaticMeshComponent>(PieceName);
			if (PieceMeshes[i])
			{
				PieceMeshes[i]->SetupAttachment(RootComponent);
				PieceMeshes[i]->SetStaticMesh(MeshAsset.Object);
				PieceMeshes[i]->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			}
		}
	}
	
 }
 
 // Called when the game starts or when spawned
 void AHM_PuzzlePiece::BeginPlay()
 {
 	Super::BeginPlay();
 	
 	bReplicates = true;
	bAlwaysRelevant = true;
	
	InitializeRandomSetting();
 }
 
 // Called every frame
 void AHM_PuzzlePiece::Tick(float DeltaTime)
 {
 	Super::Tick(DeltaTime);
 
 }

void AHM_PuzzlePiece::SetComponentOwner(UStaticMeshComponent* Component, class ATTPlayer* NewOwner)
{
	if (Component)
	{
		ComponentOwners.Add(Component, NewOwner);

	}
}

void AHM_PuzzlePiece::ClearComponentOwner(UStaticMeshComponent* Component)
{
	if (Component)
	{
		ComponentOwners.Remove(Component);
	}
}

bool AHM_PuzzlePiece::IsComponentOwned(UStaticMeshComponent* Component)
{
	return Component && ComponentOwners.Contains(Component);
}

class ATTPlayer* AHM_PuzzlePiece::GetComponentOwner(UStaticMeshComponent* Component)
{
	if (ComponentOwners.Contains(Component))
	{
		return ComponentOwners[Component];
	}
	return nullptr;
}

void AHM_PuzzlePiece::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    
	// 점수 배열 설정
	TArray<int32> ScoreOptions = {50, 50, 30, 30, 30, 20, 20, 20, 20};
	FMath::RandInit(FDateTime::Now().GetMinute());
	Algo::RandomShuffle(ScoreOptions);
	ScoreArray = ScoreOptions;

	// 물리 속성 설정
	for (int32 i = 0; i < PieceMeshes.Num(); i++)
	{
		if (PieceMeshes[i])
		{
			// 물리 및 충돌 설정
			PieceMeshes[i]->SetSimulatePhysics(true);
			PieceMeshes[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PieceMeshes[i]->SetCollisionProfileName(TEXT("PuzzlePiece"));
			PieceMeshes[i]->SetNotifyRigidBodyCollision(true);
			PieceMeshes[i]->SetGenerateOverlapEvents(true);
			PieceMeshes[i]->SetMassScale(NAME_None, 100);

			// 태그 설정
			PieceMeshes[i]->ComponentTags.Add(*FString::Printf(TEXT("Piece%d"), i + 1));

			// 점수 할당
			Pieces.Add(PieceMeshes[i], ScoreOptions[i]);
		}
	}
}

void AHM_PuzzlePiece::InitializeRandomSetting()
{
	// 각 피스의 초기 위치 설정
	FVector PiecesLocation = GetActorLocation();
	
	for(auto& Piece : Pieces)
	{
		UStaticMeshComponent* PieceComp = Piece.Key;
		int32 Score = Piece.Value;
		
		APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
		if(Manager)
		{
			Manager->AddPiece(PieceComp, Score);
		}
		
		if(PieceComp)
		{
			FVector RandomOffset = FVector(
				FMath::RandRange(-1500.f, 1500.f),
				FMath::RandRange(-1500.f, 1500.f),
				FMath::RandRange(350.f, 350.f)
			);
			
			// 기준 위치 + 랜덤 오프셋을 월드 위치로 설정
			FVector NewLocation = PiecesLocation + RandomOffset;
			PieceComp->SetWorldLocation(NewLocation);

			// 필요 시, 변환값을 저장
			//CurrentTransform = PieceComp->GetComponentTransform();
		}
	}
}

bool AHM_PuzzlePiece::AreAllPiecesDestroyed() const
{
	for (UStaticMeshComponent* MeshComponent : PieceMeshes)
	{
		if (MeshComponent && MeshComponent->IsValidLowLevel() && !MeshComponent->IsGarbageEliminationEnabled())
		{
			// 하나라도 살아있으면 false 반환
			return false;
		}
	}
	// 모든 요소가 nullptr 또는 파괴된 상태라면 true 반환
	return true;
}

void AHM_PuzzlePiece::OnRep_PieceTransform()
{
	for (auto& Piece : Pieces)
	{
		UStaticMeshComponent* PieceComp = Piece.Key;
		
		if (PieceComp && !bSimulatingPhysics)
		{
			PieceComp->SetWorldTransform(CurrentTransform);
		}
	}
}

void AHM_PuzzlePiece::OnRep_ScoreArray()
{
	// 클라이언트에서 ScoreArray를 사용하여 점수를 설정
	int32 Index = 0;
	for (auto& Piece : Pieces)
	{
		if (ScoreArray.IsValidIndex(Index))
		{
			Piece.Value = ScoreArray[Index];
			Index++;
		}
	}
}

void AHM_PuzzlePiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
	DOREPLIFETIME(AHM_PuzzlePiece, CurrentTransform);
	DOREPLIFETIME(AHM_PuzzlePiece, bSimulatingPhysics);
	DOREPLIFETIME(AHM_PuzzlePiece, ScoreArray);
}
 
