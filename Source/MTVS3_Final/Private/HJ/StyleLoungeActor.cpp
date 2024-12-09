// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/StyleLoungeActor.h"

#include "Components/BoxComponent.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayer.h"
#include "JMH/MainWidget.h"

// Sets default values
AStyleLoungeActor::AStyleLoungeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

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
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if (!TTPlayer) return;
	
	if (!TTPlayer->MainUI) return;
	
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;

	// 플레이어의 상대적 위치 판단
	FVector DoorForward = GetActorForwardVector();
	FVector PlayerRelativeLocation = TTPlayer->GetActorLocation() - GetActorLocation();
	float CrossProduct = FVector::CrossProduct(DoorForward, PlayerRelativeLocation).Z;
	
	if (ActorHasTag("StyleLoungeDoor"))
	{
		// CrossProduct > 0이면 Plaza 쪽, < 0이면 StyleLounge 쪽
		if (CrossProduct > 0)  // Plaza 쪽에서 나갈 때
		{
			GI->SetPlaceState(EPlaceState::StyleLounge);
			TTPlayer->MainUI->PlayTitleAnim(4);
		}
		else  // StyleLounge 쪽에서 나갈 때
		{
			GI->SetPlaceState(EPlaceState::Plaza);
			TTPlayer->MainUI->PlayTitleAnim(1);
		}
	}
	else if (ActorHasTag("CommunityHallDoor"))
	{
		// CrossProduct > 0이면 Plaza 쪽, < 0이면 CommunityHall 쪽
		if (CrossProduct > 0)  // Plaza 쪽에서 나갈 때
		{
			GI->SetPlaceState(EPlaceState::CommunityHall);
			TTPlayer->MainUI->PlayTitleAnim(3);
		}
		else  // CommunityHall 쪽에서 나갈 때
		{
			GI->SetPlaceState(EPlaceState::Plaza);
			TTPlayer->MainUI->PlayTitleAnim(1);
		}
	}
}
