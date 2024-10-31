// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MinimapComp.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "../../../../Plugins/2D/Paper2D/Source/Paper2D/Classes/PaperSprite.h"
#include "PaperSpriteComponent.h"

// Sets default values for this component's properties
UMH_MinimapComp::UMH_MinimapComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MinimapCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//카메라 각도 회전
	MinimapCameraBoom->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f , -90.f , 0.f)));
	//미니맵 회전 유무
	MinimapCameraBoom->bUsePawnControlRotation = false;
	MinimapCameraBoom->bInheritPitch = false;
	MinimapCameraBoom->bInheritRoll = false;
	MinimapCameraBoom->bInheritYaw = false;

	//SeceneCapture2D
	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));

	//카메라 투영타입 거리감없게
	MinimapCapture->OrthoWidth = 2000;
	//캡처된 이미지 렌더타겟 로드
	ConstructorHelpers::FObjectFinder<UCanvasRenderTarget2D> FOBJ_RenderTarget2D(
		TEXT("/Game/JMH/Tex/MH_CanRanderTarget.MH_CanRanderTarget"));
	if (FOBJ_RenderTarget2D.Succeeded())
	{
		MinimapCapture->TextureTarget = FOBJ_RenderTarget2D.Object;
	}
	
	//플레이어 위치 이미지
	MinimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapSprite"));
	MinimapSprite->SetWorldRotation(FRotator::MakeFromEuler(FVector(90.f , 0.f , -90.f)));
	MinimapSprite->SetWorldScale3D(FVector(0.5f));
	MinimapSprite->SetWorldLocation(FVector(0.f , 0.f , 300.f));
	//인게임에서는 안보이게
	MinimapSprite->bVisibleInSceneCaptureOnly = true;

	ConstructorHelpers::FObjectFinder<UPaperSprite> FOBJ_PaperSprite(TEXT("/Game/JMH/Tex/Point_Sprite1.Point_Sprite1"));
	if (FOBJ_PaperSprite.Succeeded())
	{
		MinimapSprite->SetSprite(FOBJ_PaperSprite.Object);
		MinimapSprite->SetRelativeScale3D(FVector(.8f , .8f , .8f));
	}
	// ...
}


// Called when the game starts
void UMH_MinimapComp::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* Player = Cast<ACharacter>(GetOwner());
	UPrimitiveComponent* OwnerRootComponent = Cast<UPrimitiveComponent>(Player->GetRootComponent());
	if (OwnerRootComponent)
	{
		MinimapCameraBoom->SetupAttachment(OwnerRootComponent);
		//카메라암에 붙이기
		MinimapCapture->SetupAttachment(MinimapCameraBoom);
		MinimapSprite->SetupAttachment(OwnerRootComponent);
	}
	//플레이어가 미니맵에 보이기 않게 메쉬만 숨기기
	USkeletalMeshComponent* PlayerMesh = Player->FindComponentByClass<USkeletalMeshComponent>();

	//플레이어 메쉬 숨김
	MinimapCapture->ShowFlags.SetSkeletalMeshes(false);
	// 그림자도 숨김
	// 동적 그림자 비활성화
	MinimapCapture->ShowFlags.DynamicShadows = false;
	
	
	// ...
}


// Called every frame
void UMH_MinimapComp::TickComponent(float DeltaTime , ELevelTick TickType ,
                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime , TickType , ThisTickFunction);
	ApplyMinimap();
	// ...
}

void UMH_MinimapComp::ApplyMinimap()
{
	// 오너 캐릭터 확인 및 미니맵 업데이트
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	if (Player)
	{
		//카메라 위치 업뎃
		FVector PlayerLocation = Player->GetActorLocation();
		FVector MinimapCameraLocation = FVector(PlayerLocation.X , PlayerLocation.Y , PlayerLocation.Z + 500.f);
		MinimapCameraBoom->SetWorldLocation(MinimapCameraLocation);

		MinimapSprite->SetWorldLocation(FVector(PlayerLocation.X , PlayerLocation.Y , PlayerLocation.Z + 300.f));

		//플레이어 회전값
		FRotator PlayerRotation = Player->GetActorRotation();

		// 플레이어의 카메라 회전값 가져오기
		FRotator CameraRotation;
		if (Player->GetController())
		{
			CameraRotation = Player->GetControlRotation();  // 플레이어의 카메라(컨트롤러) 회전 값
		}
		
		//미니맵 카메라의 회전을 플레이어의 Yaw 값에 맞춰 회전.
		//FRotator MinimapRotation = FRotator(-90.f, PlayerRotation.Yaw, 0.f);
		//MinimapCapture->SetWorldRotation(MinimapRotation);

		// 미니맵 캡처의 회전을 카메라의 회전 값에 맞춰 설정
		FRotator MinimapRotation = FRotator(-90.f, CameraRotation.Yaw, 0.f);
		MinimapCapture->SetWorldRotation(MinimapRotation);
		
		//플레이어의 회전 포인트 이미지에 적용
		FRotator MinimapSpriteRotation = FRotator(FRotator::MakeFromEuler(FVector(90.f , 0.f , PlayerRotation.Yaw-90.f)));
		MinimapSprite->SetWorldRotation(MinimapSpriteRotation);
	}
}
