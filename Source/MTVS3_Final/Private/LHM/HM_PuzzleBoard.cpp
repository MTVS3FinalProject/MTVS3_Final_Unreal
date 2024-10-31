// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleBoard.h"
#include "LHM/HM_PuzzlePiece.h"

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
				FVector OffsetLocation = FVector(0, Col * CellSize , Row * CellSize);
				FVector NewLocation = BoardLocation + OffsetLocation;
				BoardAreas[Idx]->SetWorldLocation(NewLocation);
			}
		}
	}
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
			BoardAreas[i]->SetupAttachment(RootComponent);
			BoardAreas[i]->SetStaticMesh(MeshAsset.Object);
			BoardAreas[i]->SetRelativeScale3D(FVector(0.7f , 7.0f , 7.0f));

			BoardAreas[i]->SetCollisionProfileName(TEXT("PuzzleBoard"));
			BoardAreas[i]->SetNotifyRigidBodyCollision(true);
			BoardAreas[i]->SetGenerateOverlapEvents(true);
			
			BoardAreas[i]->SetVisibility(false);
			//BoardAreas[i]->SetVisibility(true);
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
					UE_LOG(LogTemp, Log, TEXT("Correct piece! Board[%d] matched with %s"), i, *ActualTag);
					//BoardAreas[i]->SetVisibility(true);
					
					// 서버를 통해 동기화된 가시성 설정
					ServerSetBoardAreaVisibility(i, true);
					
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
				break;
			}
		}
	}
}

