// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePiece.h"
#include "Components/BoxComponent.h"

// Sets default values
AHM_PuzzlePiece::AHM_PuzzlePiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
    Piece = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piece"));

    RootComponent = BoxComp;
    Piece->SetupAttachment(BoxComp);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    if ( MeshAsset.Succeeded() )
    {
        Piece->SetStaticMesh(MeshAsset.Object);
    }

    BoxComp->SetSimulatePhysics(true);
    BoxComp->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
    BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void AHM_PuzzlePiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHM_PuzzlePiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

