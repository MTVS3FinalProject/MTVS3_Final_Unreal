// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawChair.h"
#include "Components/BoxComponent.h"

// Sets default values
ALuckyDrawChair::ALuckyDrawChair()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALuckyDrawChair::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALuckyDrawChair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FTransform ALuckyDrawChair::GetSittingTransform()
{
	// 의자 중심 위치 가져오기
	FVector Location = BoxComp->GetComponentLocation();
	Location.Z = 512.000069;

	// 의자 앞 방향을 기준으로 한 회전값 가져오기
	FRotator Rotation = BoxComp->GetComponentRotation();

	// 약간의 오프셋을 추가하여 플레이어가 의자와 겹치지 않게
	FVector ForwardOffset = Rotation.Vector() * SeatOffset; // 의자 앞 방향으로 SeatOffset만큼 이동

	// 최종 위치 계산
	FVector FinalLocation = Location + ForwardOffset;

	// FTransform 생성 및 반환
	return FTransform(Rotation , FinalLocation);
}
