// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleBoard.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_PuzzlePiece.h"
#include "LHM/PuzzleManager.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"

// Sets default values
AHM_PuzzleBoard::AHM_PuzzleBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoardAreas.SetNum(9);
	NiagaraEffects.SetNum(9);
	BoardAreasGrid.SetNum(9);
	
	for (int i = 0; i < 9; i++)
	{
		FString MeshAssetPath = FString::Printf(TEXT("/Game/KJM/Assets/Object/NJ__Puzzle%d"), i + 1);
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(*MeshAssetPath);
		
		FString BoardAreaCompName = FString::Printf(TEXT("BoardArea%d"), i+1);
		BoardAreas[i] = CreateDefaultSubobject<UStaticMeshComponent>(*BoardAreaCompName);
		if (BoardAreas[i])
		{
			BoardAreas[i]->SetupAttachment(RootComponent);
			BoardAreas[i]->SetStaticMesh(MeshAsset.Object);
			BoardAreas[i]->SetRelativeScale3D(FVector(2.5f));
			
			BoardAreas[i]->SetCollisionProfileName(TEXT("PuzzleBoard"));
			BoardAreas[i]->SetNotifyRigidBodyCollision(true);
			BoardAreas[i]->SetGenerateOverlapEvents(true);
			
			BoardAreas[i]->SetVisibility(false);
		}
	
		// 나이아가라 이펙트 추가
		FString NiagaraName = FString::Printf(TEXT("NiagaraEffect%d") , i + 1);
		NiagaraEffects[i] = CreateDefaultSubobject<UNiagaraComponent>(*NiagaraName);
		if (NiagaraEffects[i] && NiagaraEffectTemplate)
		{
			NiagaraEffects[i]->SetupAttachment(BoardAreas[i]);
			NiagaraEffects[i]->SetAsset(NiagaraEffectTemplate);
			NiagaraEffects[i]->SetRelativeScale3D(FVector3d(8));
			NiagaraEffects[i]->SetRelativeRotation(FRotator(0 , 0 , 90));
			NiagaraEffects[i]->SetAutoActivate(false); // 초기에는 비활성화
		}
	
		FString BoardAreasGridCompName = FString::Printf(TEXT("BoardAreasGrid_%d"), i+1);
		BoardAreasGrid[i] = CreateDefaultSubobject<UStaticMeshComponent>(*BoardAreasGridCompName);
		if (BoardAreasGrid[i])
		{
			BoardAreasGrid[i]->SetupAttachment(RootComponent);
			BoardAreasGrid[i]->SetStaticMesh(MeshAsset.Object);
			BoardAreasGrid[i]->SetRelativeScale3D(FVector(1.9f));
			
			BoardAreasGrid[i]->SetVisibility(true);
		}
	}

	for(int i = 0; i < 9; i++)
	{
		BoardAreas[i]->OnComponentBeginOverlap.AddDynamic(this, &AHM_PuzzleBoard::OnOverlapBegin);
	}
}

// Called when the game starts or when spawned
void AHM_PuzzleBoard::BeginPlay()
{
	Super::BeginPlay();

	bReplicates = true;
    
	// 서버에서만 초기화
	if (HasAuthority())
	{
		// BoardAreaVisibility 초기화
		BoardAreaVisibility.Init(false, 9);
        
		// 초기 가시성 상태 설정
		for (int32 i = 0; i < BoardAreas.Num(); i++)
		{
			if (BoardAreas[i])
			{
				BoardAreas[i]->SetVisibility(false);
				BoardAreaVisibility[i] = false;
			}
		}
	}
    
	InitializeBoardAreasLocation();
	InitializeBoardAreasGridLocation();
}

// Called every frame
void AHM_PuzzleBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
			int32 Idx = Col * GridSize + Row;
			if (BoardAreas.IsValidIndex(Idx) && BoardAreas[Idx])
			{
				FVector OffsetLocation = FVector(Row * CellSize , 0, -Col * CellSize);
				FVector NewLocation = BoardLocation + OffsetLocation;
				BoardAreas[Idx]->SetWorldLocation(NewLocation);

				FVector NiagaraOffsetLocation = FVector(Row * CellSize , 100, -Col * CellSize);
				FVector NewNiagaraLocation = BoardLocation + NiagaraOffsetLocation;
				NiagaraEffects[Idx]->SetWorldLocation(NewNiagaraLocation);
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
			int32 Idx = Col * GridSize + Row;
			if (BoardAreasGrid.IsValidIndex(Idx) && BoardAreasGrid[Idx])
			{
				FVector OffsetLocation = FVector(Row * CellSize , 0, -Col * CellSize);
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
		// BoardAreaVisibility 업데이트
		if (HasAuthority())
		{
			BoardAreaVisibility[BoardIndex] = bVisible;
			NiagaraEffects[BoardIndex]->Activate(true);
			UE_LOG(LogTemp, Log, TEXT("Updated BoardAreaVisibility[%d] to %s"), BoardIndex, bVisible ? TEXT("true") : TEXT("false"));

			// 모든 보드 영역이 가시화되었는지 확인
			if (AreAllBoardAreasVisible())
			{
				UE_LOG(LogTemp, Log, TEXT("All board areas are visible. Ending puzzle."));
				// 퍼즐 완료 처리
				APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
				if (Manager)
				{
					Manager->SortAndUpdateRanking(); // 퍼즐 종료 로직 호출
				}
			}
		}
        
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

		if (NiagaraEffects.IsValidIndex(BoardIndex) && NiagaraEffects[BoardIndex])
		{   
			if (bVisible)
			{
				NiagaraEffects[BoardIndex]->Activate(true);
			}
			else
			{
				NiagaraEffects[BoardIndex]->Deactivate();
			}
			UE_LOG(LogTemp, Log, TEXT("Niagara Effect for BoardArea[%d] %s"), BoardIndex, bVisible ? TEXT("Activated") : TEXT("Deactivated"));
		}
        
		if (HasAuthority())
		{
			BoardAreaVisibility[BoardIndex] = bVisible;
		}
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
					
					APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
					if(Manager)
					{
						Manager->PlayHit();
						
						// OtherComp를 UStaticMeshComponent로 캐스팅
						UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(OtherComp);
						if (MeshComp && PuzzlePiece->LastOwners.Contains(MeshComp))
						{
							// LastOwners에서 닉네임 가져오기
							FString PlayerNickname = PuzzlePiece->LastOwners[MeshComp];
							if (!PlayerNickname.IsEmpty())
							{
								// 피스의 점수 가져오기
								int32 PieceScore = Manager->GetPieceScore(MeshComp);
								
								GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
																 FString::Printf(TEXT("Hit Piece Score : %d"), PieceScore));
								UE_LOG(LogTemp, Log, TEXT("Player %s scored %d points."), *PlayerNickname, PieceScore);

								// 마지막 소유자에게 점수 부여
								Manager->AddScoreToPlayer(PlayerNickname, PieceScore);
							}
						}
					}
					// 서버를 통해 동기화된 가시성 설정
					ServerSetBoardAreaVisibility(i, true);

					// 맞춘 피스는 제거
					if(HasAuthority())
					{
						MulticastDestroyPuzzlePiece(PuzzlePiece, ActualTag);
					}
				}
				break;
			}
		}
	}
}

bool AHM_PuzzleBoard::AreAllBoardAreasVisible() const
{
	// BoardAreaVisibility 배열의 모든 값이 true인지 확인
	for (bool bVisible : BoardAreaVisibility)
	{
		if (!bVisible)
		{
			return false; // 하나라도 false면 전체가 true가 아님
		}
	}
	UE_LOG(LogTemp, Log, TEXT("All BoardAreaVisibility values are true"));
	return true; // 모든 값이 true임
}

void AHM_PuzzleBoard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHM_PuzzleBoard, BoardAreaVisibility);
	DOREPLIFETIME(AHM_PuzzleBoard, DestroyedPieceTags);
}

void AHM_PuzzleBoard::OnRep_BoardAreaVisibility()
{
	for (int32 i = 0; i < BoardAreaVisibility.Num(); i++)
	{
		if (BoardAreas.IsValidIndex(i) && BoardAreas[i])
		{
			BoardAreas[i]->SetVisibility(BoardAreaVisibility[i]);
		}
	}
}

void AHM_PuzzleBoard::OnRep_DestroyedPieceTags()
{
	// 월드에서 모든 퍼즐 피스를 찾아서 확인
	TArray<AActor*> FoundPuzzlePieces;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHM_PuzzlePiece::StaticClass(), FoundPuzzlePieces);

	for (AActor* Actor : FoundPuzzlePieces)
	{
		AHM_PuzzlePiece* PuzzlePiece = Cast<AHM_PuzzlePiece>(Actor);
		if (PuzzlePiece)
		{
			const TArray<UStaticMeshComponent*>& PiecesArray = PuzzlePiece->GetAllPieces();
			for (UStaticMeshComponent* Piece : PiecesArray)
			{
				if (Piece && Piece->ComponentTags.Num() > 0)
				{
					FString PieceTag = Piece->ComponentTags[0].ToString();
					if (DestroyedPieceTags.Contains(PieceTag))
					{
						Piece->DestroyComponent();
					}
				}
			}
		}
	}
}

void AHM_PuzzleBoard::MulticastDestroyPuzzlePiece_Implementation(AHM_PuzzlePiece* PuzzlePiece,
                                                                 const FString& TagToDestroy)
{
	if (PuzzlePiece)
	{
		if (HasAuthority())
		{
			// 서버에서만 DestroyedPieceTags 업데이트
			DestroyedPieceTags.AddUnique(TagToDestroy);
		}

		const TArray<UStaticMeshComponent*>& PiecesArray = PuzzlePiece->GetAllPieces();
		for(UStaticMeshComponent* Piece : PiecesArray)
		{
			if(Piece && Piece->ComponentTags.Num() > 0 &&
			   Piece->ComponentTags[0].ToString() == TagToDestroy)
			{
				Piece->DestroyComponent();
				break;
			}
		}
	}
}

