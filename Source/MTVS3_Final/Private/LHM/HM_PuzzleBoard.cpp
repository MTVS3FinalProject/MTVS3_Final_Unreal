// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleBoard.h"

#include "Kismet/GameplayStatics.h"
#include "LHM/HM_PuzzlePiece.h"
#include "LHM/PuzzleManager.h"

// Sets default values
AHM_PuzzleBoard::AHM_PuzzleBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InitializeBoardAreas();

	for(int i = 0; i < 9; i++)
	{
		BoardAreas[i]->OnComponentBeginOverlap.AddDynamic(this, &AHM_PuzzleBoard::OnOverlapBegin);
	}
}

// Called when the game starts or when spawned
void AHM_PuzzleBoard::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 리플리케이션 활성화
	bReplicates = true;
	
	InitializeBoardAreasLocation();
	InitializeBoardAreasGridLocation();
	
}

// Called every frame
void AHM_PuzzleBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_PuzzleBoard::InitializeBoardAreas()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if(!MeshAsset.Succeeded()) return;
	
	BoardAreas.SetNum(9);
	for (int i = 0; i < 9; i++)
	{
		FString ComponentName = FString::Printf(TEXT("BoardArea%d"), i+1);
		BoardAreas[i] = CreateDefaultSubobject<UStaticMeshComponent>(*ComponentName);

		if (BoardAreas[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("Setting scale for BoardAreas[%d]"), i);
			BoardAreas[i]->SetupAttachment(RootComponent);
			BoardAreas[i]->SetStaticMesh(MeshAsset.Object);
			BoardAreas[i]->SetRelativeScale3D(FVector(0.7f , 7.0f , 7.0f));
			BoardAreas[i]->SetRelativeRotation(FRotator(0,90,0));
			
			BoardAreas[i]->SetCollisionProfileName(TEXT("PuzzleBoard"));
			BoardAreas[i]->SetNotifyRigidBodyCollision(true);
			BoardAreas[i]->SetGenerateOverlapEvents(true);
			
			BoardAreas[i]->SetVisibility(false);
		}
	}

	BoardAreasGrid.SetNum(9);
	for (int i = 0; i < 9; i++)
	{
		FString ComponentName = FString::Printf(TEXT("BoardAreasGrid_%d"), i+1);
		BoardAreasGrid[i] = CreateDefaultSubobject<UStaticMeshComponent>(*ComponentName);

		if (BoardAreasGrid[i])
		{
			UE_LOG(LogTemp, Warning, TEXT("Setting scale for BoardAreasGrid[%d]"), i);
			BoardAreasGrid[i]->SetupAttachment(RootComponent);
			BoardAreasGrid[i]->SetStaticMesh(MeshAsset.Object);
			//BoardAreasGrid[i]->SetRelativeLocation(FVector(0.0f, 10.f, 0.0f));
			BoardAreasGrid[i]->SetRelativeScale3D(FVector(0.1f , 6.0f , 6.0f));
			BoardAreasGrid[i]->SetRelativeRotation(FRotator(0,90,0));
			
			BoardAreasGrid[i]->SetVisibility(true);
		}
	}
}

void AHM_PuzzleBoard::InitializeBoardAreasLocation()
{
	FVector BoardLocation = GetActorLocation();
	// 3x3 형태로 퍼즐 보드 위치를 설정
	const int32 GridSize = 3;
	for (int32 Row = 0; Row < GridSize; ++Row)
	{
		for (int32 Col = 0; Col < GridSize; ++Col)
		{
			int32 Idx = Row * GridSize + Col;
			if (BoardAreas.IsValidIndex(Idx) && BoardAreas[Idx])
			{
				FVector OffsetLocation = FVector(Col * -CellSize , 0, Row * CellSize);
				FVector NewLocation = BoardLocation + OffsetLocation;
				BoardAreas[Idx]->SetWorldLocation(NewLocation);
			}
		}
	}
}

void AHM_PuzzleBoard::InitializeBoardAreasGridLocation()
{
	FVector BoardGridLocation = GetActorLocation();
	// 3x3 형태로 퍼즐 보드 위치를 설정
	const int32 GridSize = 3;
	for (int32 Row = 0; Row < GridSize; ++Row)
	{
		for (int32 Col = 0; Col < GridSize; ++Col)
		{
			int32 Idx = Row * GridSize + Col;
			if (BoardAreasGrid.IsValidIndex(Idx) && BoardAreasGrid[Idx])
			{
				FVector OffsetLocation = FVector(Col * -CellSize , 0, Row * CellSize);
				FVector NewLocation = BoardGridLocation + OffsetLocation;
				BoardAreasGrid[Idx]->SetWorldLocation(NewLocation);
			}
		}
	}
}

void AHM_PuzzleBoard::ServerSetBoardAreaVisibility_Implementation(int32 BoardIndex, bool bVisible)
{
	// 서버에서 유효성 검사
	if (BoardIndex >= 0 && BoardIndex < BoardAreas.Num())
	{
		// 모든 클라이언트(및 서버)에게 알림
		MulticastSetBoardAreaVisibility(BoardIndex, bVisible);
	}
}

void AHM_PuzzleBoard::MulticastSetBoardAreaVisibility_Implementation(int32 BoardIndex, bool bVisible)
{
	// 모든 클라이언트와 서버에서 실행
	if (BoardIndex >= 0 && BoardIndex < BoardAreas.Num())
	{
		BoardAreas[BoardIndex]->SetVisibility(bVisible);
	}
}

void AHM_PuzzleBoard::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHM_PuzzlePiece* PuzzlePiece = Cast<AHM_PuzzlePiece>(OtherActor);
	if(PuzzlePiece && OtherComp && OtherComp->ComponentTags.Num() > 0)
	{
		for(int32 i = 0; i < BoardAreas.Num(); i++)
		{
			if(OverlappedComponent == BoardAreas[i])
			{
				FString ExpectedTag = FString::Printf(TEXT("Piece%d"), i + 1);
				FString ActualTag = OtherComp->ComponentTags[0].ToString();

				if(ActualTag == ExpectedTag)
				{
					UE_LOG(LogTemp, Log, TEXT("Correct piece! Board[%d] matched with %s"), i+1, *ActualTag);
					
					// 서버를 통해 동기화된 가시성 설정
					ServerSetBoardAreaVisibility(i, true);

					APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
					if(Manager)
					{
						// OtherComp를 UStaticMeshComponent로 캐스팅
						UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(OtherComp);
						if (MeshComp && PuzzlePiece->LastOwners.Contains(MeshComp))
						{
							AActor* LastOwner = PuzzlePiece->LastOwners[MeshComp];
							if(LastOwner)
							{
								// 피스의 점수 가져오기
								int32 PieceScore = Manager->GetPieceScore(MeshComp);
								
								GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Hit Piece Score : %d"),PieceScore));
                            
								// 마지막 소유자에게 점수 부여
								Manager->AddScoreToPlayer(LastOwner, PieceScore);
							}
						}
					}

					// 맞춘 피스는 제거
					if(HasAuthority())
					{
						const TArray<UStaticMeshComponent*>& PiecesArray = PuzzlePiece->GetAllPieces();
						for(UStaticMeshComponent* Piece : PiecesArray)
						{
							if(Piece && Piece->ComponentTags.Num() > 0 &&
							   Piece->ComponentTags[0].ToString() == ActualTag)
							{
								Piece->DestroyComponent();
								break;
							}
						}	
					}
				}
				break;
			}
		}
	}
}

