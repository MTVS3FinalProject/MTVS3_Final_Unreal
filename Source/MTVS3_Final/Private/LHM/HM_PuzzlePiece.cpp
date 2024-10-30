// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePiece.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHM_PuzzlePiece::AHM_PuzzlePiece()
 {
  	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

	InitializePieces();
    // Piece = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piece"));
    //
    // Piece->SetupAttachment(RootComponent);
    //
    // static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
	   //  TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    // if (MeshAsset.Succeeded())
    // {
	   //  Piece->SetStaticMesh(MeshAsset.Object);
    // }
    // Piece->SetRelativeLocation(FVector(0 , 90 , 0));
    // Piece->SetRelativeScale3D(FVector(0.2 , 0.2 , 0.2));
    // Piece->SetSimulatePhysics(true);
    // Piece->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    // Piece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 }
 
 // Called when the game starts or when spawned
 void AHM_PuzzlePiece::BeginPlay()
 {
 	Super::BeginPlay();
 	
 	bReplicates = true;
	
	// 각 피스의 초기 위치 설정
	for(auto* Piece : Pieces)
	{
		if(Piece)
		{
			FVector RandomLocation = FVector(
				FMath::RandRange(1000.f, 1500.f),
				FMath::RandRange(1500.f, 2000.f),
				FMath::RandRange(350.f, 350.f)
				//(X=1350.000000,Y=1820.000000,Z=340.000000)
			);
			Piece->SetRelativeLocation(RandomLocation);
		}
	}
 
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
	for (UStaticMeshComponent* Piece : Pieces)
	{
		if (Piece && !bSimulatingPhysics)
		{
			Piece->SetWorldTransform(CurrentTransform);
		}
	}

	// if (GetPiece() && !bSimulatingPhysics)
 	// {
 	// 	GetPiece()->SetWorldTransform(CurrentTransform);
 	// }
 }

void AHM_PuzzlePiece::InitializePieces()
{
	UE_LOG(LogTemp, Log, TEXT("AHM_PuzzlePiece::InitializePieces"));
	// Static Mesh 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if(!MeshAsset.Succeeded()) return;

	for(int32 i = 0; i < 9; i++)
	{
		// UStaticMeshComponent 생성 및 설정
		FName PieceName = *FString::Printf(TEXT("Piece%d"), i + 1);
		UStaticMeshComponent* NewPiece = CreateDefaultSubobject<UStaticMeshComponent>(PieceName);
		if(NewPiece)
		{
			NewPiece->SetupAttachment(RootComponent);
			NewPiece->SetStaticMesh(MeshAsset.Object);
			NewPiece->SetRelativeScale3D(FVector(0.5f , 0.5f , 0.5f));

			// 물리 및 충돌 설정
			NewPiece->SetSimulatePhysics(true);
			NewPiece->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
			NewPiece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			// 태그 설정
			NewPiece->ComponentTags.Add(PieceName);

			// Pieces 배열에 추가
			Pieces.Add(NewPiece);
		}
	}
}
