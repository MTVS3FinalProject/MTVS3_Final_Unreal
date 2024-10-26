// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePiece.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHM_PuzzlePiece::AHM_PuzzlePiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Piece = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piece"));

    //Piece->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    if ( MeshAsset.Succeeded() )
    {
        Piece->SetStaticMesh(MeshAsset.Object);
    }

    Piece->SetSimulatePhysics(true);
    Piece->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    Piece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void AHM_PuzzlePiece::BeginPlay()
{
	Super::BeginPlay();
	
	bReplicates = true;

}

// Called every frame
void AHM_PuzzlePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHM_PuzzlePiece::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHM_PuzzlePiece, CurrentTransform);
	DOREPLIFETIME(AHM_PuzzlePiece, bSimulatingPhysics);
}

void AHM_PuzzlePiece::OnRep_PieceTransform()
{
	if (GetPiece() && !bSimulatingPhysics)
	{
		GetPiece()->SetWorldTransform(CurrentTransform);
	}
}

// void AHM_PuzzlePiece::Server_SetPieceLocation_Implementation(const FVector& NewLocation)
// {
// 	// 새로운 위치를 설정하고 동기화
// 	CurrentTransform.SetLocation(NewLocation);
// 	OnRep_PieceTransform(); // 동기화 함수 호출
// }
//
// // 퍼즐 조각을 놓을 때 이 함수를 호출
// void AHM_PuzzlePiece::PlacePiece(const FVector& NewLocation)
// {
// 	if (HasAuthority()) // 서버에서 직접 호출된 경우
// 	{
// 		CurrentTransform.SetLocation(NewLocation);
// 		OnRep_PieceTransform();
// 	}
// 	else // 클라이언트에서 호출된 경우
// 	{
// 		Server_SetPieceLocation(NewLocation); // 서버에 요청
// 	}
// }




