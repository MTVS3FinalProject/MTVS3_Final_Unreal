// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawChair.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

// Sets default values
ALuckyDrawChair::ALuckyDrawChair()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(BoxComp);
}

// Called when the game starts or when spawned
void ALuckyDrawChair::BeginPlay()
{
	Super::BeginPlay();

	// 초기 위치와 회전값 저장
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
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
	Location.Z += 52;

	// 의자 앞 방향을 기준으로 한 회전값 가져오기
	FRotator Rotation = BoxComp->GetComponentRotation();

	// 약간의 오프셋을 추가하여 플레이어가 의자와 겹치지 않게
	FVector ForwardOffset = Rotation.Vector() * SeatOffset; // 의자 앞 방향으로 SeatOffset만큼 이동

	// 최종 위치 계산
	FVector FinalLocation = Location + ForwardOffset;

	// FTransform 생성 및 반환
	return FTransform(Rotation , FinalLocation);
}

void ALuckyDrawChair::ResetChair()
{
	// 원래 위치와 회전값으로 리셋
	SetActorLocationAndRotation(
		OriginalLocation , // BeginPlay에서 저장해둔 초기 위치
		OriginalRotation , // BeginPlay에서 저장해둔 초기 회전값
		false , // 물리적 충돌 체크 안함
		nullptr , // 충돌 결과 필요 없음
		ETeleportType::ResetPhysics // 물리 상태 리셋
	);
}

void ALuckyDrawChair::MulticastSetPhysicsState_Implementation(bool bSimulate)
{
	if (BoxComp)
	{
		BoxComp->SetSimulatePhysics(bSimulate);
	}
}

// 새로운 함수 구현
void ALuckyDrawChair::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MeshComp)
	{
		InitialMeshTransform = MeshComp->GetRelativeTransform();
	}
}

void ALuckyDrawChair::MulticastResetChair_Implementation()
{
	if (!BoxComp) return;
    
	// 1. 모든 물리 시뮬레이션 완전히 정지
	BoxComp->SetSimulatePhysics(false);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
	// 2. 속도 초기화
	BoxComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
	BoxComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    
	// 3. 위치 리셋
	SetActorLocationAndRotation(
		OriginalLocation,
		OriginalRotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics  // ResetPhysics 대신 TeleportPhysics 사용
	);
    
	// 4. Mesh 위치 리셋
	if (MeshComp)
	{
		MeshComp->SetRelativeTransform(InitialMeshTransform);
	}
    
	// 5. 물리 상태 복원
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
