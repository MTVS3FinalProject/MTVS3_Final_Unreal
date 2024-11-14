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
	
	SetupCollisionProfiles();
	InitializeRandomSetting();

	// KHJ
	// 서버에서만 주기적으로 Transform 업데이트
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			TransformUpdateTimer,
			this,
			&AHM_PuzzlePiece::UpdateServerTransforms,
			0.1f, // 0.1초마다 업데이트
			true
		);
	}
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
			PieceMeshes[i]->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
			
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
	if (HasAuthority())
    {
        FVector PiecesLocation = GetActorLocation();
        ServerTransforms.SetNum(PieceMeshes.Num());
        
        // 매니저 찾기
        APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
        
        // Pieces 맵의 각 요소에 대해 초기화
        for(auto& Piece : Pieces)
        {
            UStaticMeshComponent* PieceComp = Piece.Key;
            int32 Score = Piece.Value;
            
            // 매니저에 피스 등록
            if(Manager)
            {
                Manager->AddPiece(PieceComp, Score);
            }
            
            if(PieceComp)
            {
                // 랜덤 위치 계산
                FVector RandomOffset = FVector(
                    FMath::RandRange(-1500.f, 1500.f),
                    FMath::RandRange(-1500.f, 1500.f),
                    FMath::RandRange(500.f, 500.f)
                );
                
            	FVector NewLocation = PiecesLocation + RandomOffset;
            	// FTransform NewTransform = FTransform(FRotator::ZeroRotator, NewLocation);
            	
				 FTransform NewTransform = FTransform(
					FRotator::ZeroRotator, 
					NewLocation,
					FVector(0.75f, 0.75f, 0.75f)  // 스케일 설정
				);
                
                // 현재 피스의 인덱스 찾기
                int32 Index = PieceMeshes.Find(PieceComp);
                if(Index != INDEX_NONE)
                {
                    ServerTransforms[Index] = NewTransform;
                }
                
                // 월드 위치 설정
                PieceComp->SetWorldTransform(NewTransform);
            }
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

	DOREPLIFETIME(AHM_PuzzlePiece, ServerTransforms);
	DOREPLIFETIME(AHM_PuzzlePiece, CurrentTransform);
	DOREPLIFETIME(AHM_PuzzlePiece, bSimulatingPhysics);
	DOREPLIFETIME(AHM_PuzzlePiece, ScoreArray);
}

void AHM_PuzzlePiece::OnRep_ServerTransforms()
{
	if (!HasAuthority())
	{
		for (int32 i = 0; i < PieceMeshes.Num(); i++)
		{
			if (PieceMeshes[i] && ServerTransforms.IsValidIndex(i))
			{
				// 물리 시뮬레이션은 유지하면서 위치만 부드럽게 보간
				FVector CurrentLocation = PieceMeshes[i]->GetComponentLocation();
				FVector TargetLocation = ServerTransforms[i].GetLocation();
                
				// 급격한 위치 변화가 있을 때만 위치 강제 설정
				float DistanceSquared = FVector::DistSquared(CurrentLocation, TargetLocation);
				if (DistanceSquared > 10000.0f) // 100 units 이상 차이날 때
				{
					PieceMeshes[i]->SetWorldTransform(ServerTransforms[i]);
				}
				else if (DistanceSquared > 1.0f) // 작은 차이는 부드럽게 보간
				{
					FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), 10.0f);
					PieceMeshes[i]->SetWorldLocation(NewLocation);
				}
                
				// 회전도 부드럽게 보간
				FRotator CurrentRotation = PieceMeshes[i]->GetComponentRotation();
				FRotator TargetRotation = ServerTransforms[i].Rotator();
				FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.0f);
				PieceMeshes[i]->SetWorldRotation(NewRotation);
			}
		}
	}
}

void AHM_PuzzlePiece::UpdateServerTransforms()
{
	if (HasAuthority())
	{
		bool bHasChanged = false;
		ServerTransforms.SetNum(PieceMeshes.Num());
        
		for (int32 i = 0; i < PieceMeshes.Num(); i++)
		{
			if (PieceMeshes[i])
			{
				FTransform NewTransform = PieceMeshes[i]->GetComponentTransform();
                
				// Transform이 변경되었는지 확인
				if (!ServerTransforms[i].Equals(NewTransform))
				{
					ServerTransforms[i] = NewTransform;
					bHasChanged = true;
				}
			}
		}
        
		// 변경된 경우에만 클라이언트에 전파
		if (bHasChanged)
		{
			MulticastUpdateTransforms();
		}
	}
}

void AHM_PuzzlePiece::SetupCollisionProfiles()
{
	// 각 피스의 충돌 설정
	for (auto* Piece : PieceMeshes)
	{
		if (Piece)
		{
			// 물리 시뮬레이션 설정
			Piece->SetSimulatePhysics(true);
			Piece->SetEnableGravity(true);
            
			// 충돌 설정 - 플레이어와 Block으로 변경
			Piece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Piece->SetCollisionObjectType(ECC_PhysicsBody);
			Piece->SetCollisionResponseToAllChannels(ECR_Block);
            
			// 물리 속성 조정
			Piece->SetLinearDamping(0.5f);  // 움직임 감쇠
			Piece->SetAngularDamping(0.5f); // 회전 감쇠
			Piece->SetMassScale(NAME_None, 10.0f); // 충돌 시 잘 밀리도록
		}
	}
}

void AHM_PuzzlePiece::MulticastUpdateTransforms_Implementation()
{
		if (!HasAuthority())
		{
			OnRep_ServerTransforms();
		}
}
