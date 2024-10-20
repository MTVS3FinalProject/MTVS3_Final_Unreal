// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MinimapComp.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UMH_MinimapComp::UMH_MinimapComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	MinimapCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//MinimapCameraBoom->SetupAttachment(GetOwner()->GetRootComponent());
	//카메라 각도 회전
	MinimapCameraBoom->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f,-90.f,0.f)));
	//미니맵 회전 유무
	MinimapCameraBoom->bUsePawnControlRotation = true;
	MinimapCameraBoom->bInheritPitch = false;
	MinimapCameraBoom->bInheritRoll = false;
	MinimapCameraBoom->bInheritYaw = false;

	//SeceneCapture2D
	MinimapCapture= CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	//카메라암에 붙이기
	MinimapCapture->SetupAttachment(MinimapCameraBoom);
	//카메라 투영타입 거리감없게
	MinimapCapture->OrthoWidth = 3072;
	//캡처된 이미지 렌더타겟 로드
	ConstructorHelpers::FObjectFinder<UCanvasRenderTarget2D> MH_RenderTarget2D(TEXT("/Game/JMH/Tex/MH_CanRanderTarget.MH_CanRanderTarget"));
	if(MH_RenderTarget2D.Succeeded())
	{
		MinimapCapture->TextureTarget = MH_RenderTarget2D.Object;
	}

	//플레이어 위치 이미지
	MinimapTexture = CreateDefaultSubobject<UTexture>(TEXT("MinimapTexture"));
	//MinimapTexture->
	
	// ...
}


// Called when the game starts
void UMH_MinimapComp::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<ACharacter>(GetOwner());
	// ...
	
	
}


// Called every frame
void UMH_MinimapComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMH_MinimapComp::ApplyMinimap()
{
	
}
