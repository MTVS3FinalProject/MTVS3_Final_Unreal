// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawTable.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

// Sets default values
ALuckyDrawTable::ALuckyDrawTable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

	TableMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMeshComp"));
	TableMeshComp->SetupAttachment(BoxComp);
	TableMeshComp->SetSimulatePhysics(false);

	FloorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComp"));
	FloorMeshComp->SetupAttachment(BoxComp);
	FloorMeshComp->SetSimulatePhysics(false);

	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComp"));
	TextRenderComp->SetupAttachment(TableMeshComp);
}

// Called when the game starts or when spawned
void ALuckyDrawTable::BeginPlay()
{
	Super::BeginPlay();

	// 초기 위치와 회전값 저장
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();

	// BoxComp의 물리 속성 설정
	if (BoxComp)
	{
		BoxComp->SetSimulatePhysics(false);
		BoxComp->SetEnableGravity(true);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// TextRenderComponent가 MeshComp를 강제로 따라가도록 설정
	if (TextRenderComp && TableMeshComp)
	{
		// TextRenderComponent를 MeshComponent에 직접 연결
		TextRenderComp->AttachToComponent(TableMeshComp , FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void ALuckyDrawTable::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 초기 Transform 저장
	if (TableMeshComp)
	{
		InitialTableMeshTransform = TableMeshComp->GetRelativeTransform();
	}

	if (FloorMeshComp)
	{
		InitialFloorMeshTransform = FloorMeshComp->GetRelativeTransform();
	}

	if (TextRenderComp)
	{
		InitialTextRenderTransform = TextRenderComp->GetRelativeTransform();
	}
}

// Called every frame
void ALuckyDrawTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALuckyDrawTable::MulticastSetTextRender_Implementation(const FText& _RandomSeatNumber)
{
	TextRenderComp->SetText(_RandomSeatNumber);
}

void ALuckyDrawTable::MulticastSetPhysicsState_Implementation(bool bSimulate)
{
	if (!BoxComp) return;

	BoxComp->SetSimulatePhysics(bSimulate);

	if (bSimulate)
	{
		// 물리 시뮬레이션 활성화 시 충돌 활성화
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PlaySpark();
	}
	else
	{
		// 물리 시뮬레이션 비활성화 시 쿼리만 가능하도록 설정
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void ALuckyDrawTable::MulticastResetTable_Implementation()
{
	if (!BoxComp) return;

	// 1. 물리/충돌 완전 비활성화
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. 모든 물리 상태 초기화
	BoxComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	BoxComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	BoxComp->SetAllPhysicsPosition(OriginalLocation);
	BoxComp->SetAllPhysicsRotation(OriginalRotation);

	// 3. Actor의 Transform 리셋
	SetActorLocationAndRotation(
		OriginalLocation ,
		OriginalRotation ,
		false ,
		nullptr ,
		ETeleportType::TeleportPhysics
	);

	// 4. 각 메시 컴포넌트의 Transform 리셋
	if (TableMeshComp)
	{
		TableMeshComp->SetRelativeTransform(InitialTableMeshTransform);
		TableMeshComp->SetSimulatePhysics(false);
		TableMeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		TableMeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	if (FloorMeshComp)
	{
		FloorMeshComp->SetRelativeTransform(InitialFloorMeshTransform);
		FloorMeshComp->SetSimulatePhysics(false);
		FloorMeshComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		FloorMeshComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}

	// 5. TextRender 컴포넌트 리셋
	if (TextRenderComp)
	{
		TextRenderComp->SetRelativeTransform(InitialTextRenderTransform);
	}

	// 6. 충돌 재활성화
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 디버그 로그
	UE_LOG(LogTemp , Log , TEXT("Table %s reset to original position: %s") ,
	       *GetName() , *OriginalLocation.ToString());
}
