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
	
	InitializePieces();
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

void AHM_PuzzlePiece::InitializePieces()
{
	if (HasAuthority())  // 서버에서만 실행
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
			TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
		if (!MeshAsset.Succeeded()) return;

		// 점수 배열 설정하고 배열을 섞어서 랜덤으로 배치
		TArray<int32> ScoreOptions = {50 , 50 , 30 , 30 , 30 , 20 , 20 , 20 , 20};
		FMath::RandInit(FDateTime::Now().GetMillisecond());
		Algo::RandomShuffle(ScoreOptions);

		ScoreArray = ScoreOptions;  // 클라이언트 동기화를 위한 ScoreArray 설정
		
		for (int32 i = 0; i < 9; i++)
		{
			// UStaticMeshComponent 생성 및 설정
			FName PieceName = *FString::Printf(TEXT("Piece%d") , i + 1);
			UStaticMeshComponent* NewPiece = CreateDefaultSubobject<UStaticMeshComponent>(PieceName);
			if (NewPiece)
			{
				//NewPiece->SetupAttachment(RootComponent);
				NewPiece->SetStaticMesh(MeshAsset.Object);
				NewPiece->SetRelativeScale3D(FVector(0.5f , 0.5f , 0.5f));

				// 물리 및 충돌 설정
				NewPiece->SetSimulatePhysics(true);
				NewPiece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				NewPiece->SetCollisionProfileName(TEXT("PuzzlePiece"));
				NewPiece->SetNotifyRigidBodyCollision(true);
				NewPiece->SetGenerateOverlapEvents(true);

				// 태그 설정
				NewPiece->ComponentTags.Add(PieceName);

				// 랜덤 점수 할당하고 Pieces 배열에 추가
				int32 InitialScore = ScoreOptions[i];
				Pieces.Add(NewPiece , InitialScore);

				//UE_LOG(LogTemp , Log , TEXT("Assigned score %d to %s") , InitialScore , *PieceName.ToString());
			}
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
				FMath::RandRange(-1000.f, 1000.f),
				FMath::RandRange(-1000.f, 1000.f),
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
 
