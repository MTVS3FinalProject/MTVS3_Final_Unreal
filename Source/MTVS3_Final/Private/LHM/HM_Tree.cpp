// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_Tree.h"

// Sets default values
AHM_Tree::AHM_Tree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree"));
	Tree->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TicatAsset(
		TEXT("/Game/Greek_island/Assets/Fish/Ticat1"));
	if (TicatAsset.Succeeded())
	{
		Ticats.SetNum(10);
		for (int32 i = 0; i < 10; i++)
		{
			FName TicatName = *FString::Printf(TEXT("Ticat_%d"), i + 1);
			UStaticMeshComponent* TicatComp = CreateDefaultSubobject<UStaticMeshComponent>(TicatName);
			Ticats[i] = TicatComp;
            
			if (TicatComp)
			{
				TicatComp->SetupAttachment(Tree);
				TicatComp->SetStaticMesh(TicatAsset.Object);
				TicatComp->SetRelativeRotation(FRotator(90, 0, 90));
			}
		}
	}
}

// Called when the game starts or when spawned
void AHM_Tree::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < 10; i++)
	{
		Ticats[i]->SetVisibility(false);
	}
}

// Called every frame
void AHM_Tree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

