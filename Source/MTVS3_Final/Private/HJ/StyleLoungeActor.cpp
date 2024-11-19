// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/StyleLoungeActor.h"

#include "Components/BoxComponent.h"
#include "HJ/TTGameInstance.h"

// Sets default values
AStyleLoungeActor::AStyleLoungeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

	BoxComp->OnComponentBeginOverlap.AddDynamic(this , &AStyleLoungeActor::OnBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this , &AStyleLoungeActor::OnEndOverlap);
}

// Called when the game starts or when spawned
void AStyleLoungeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStyleLoungeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStyleLoungeActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;
	if (GI->GetPlaceState() == EPlaceState::Plaza) GI->SetPlaceState(EPlaceState::StyleLounge);
	else GI->SetPlaceState(EPlaceState::Plaza);
}
