// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MinimapActor.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMH_MinimapActor::AMH_MinimapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Camera SpringArm
	MinimapCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//카메라 회전
	MinimapCameraBoom->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f , -90.f , 0.f)));
	//미니맵 회전 유무
	MinimapCameraBoom->bUsePawnControlRotation = false;
	MinimapCameraBoom->bInheritPitch = false;
	MinimapCameraBoom->bInheritRoll = false;
	MinimapCameraBoom->bInheritYaw = false;

	//SeceneCapture2D 생성
	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	//카메라 투영타입 거리감없게
	MinimapCapture->OrthoWidth = 2000;
	MinimapCapture->SetupAttachment(MinimapCameraBoom);
	
	//플레이어 위치 이미지
	MinimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapSprite"));
	MinimapSprite->SetWorldRotation(FRotator(90.f , 0.f , -90.f)); // Set rotation for top-down view
	MinimapSprite->SetWorldScale3D(FVector(1.f));
	MinimapSprite->SetWorldLocation(FVector(0.f , 0.f , 300.f));
	
	//인게임에서는 안보이게
	MinimapSprite->bVisibleInSceneCaptureOnly = true;
	
	RenderTarget = NewObject<UTextureRenderTarget2D>();
	//RenderTarget->InitAutoFormat(1024, 1024); // 1024x1024 크기로 초기화
	MinimapCapture->TextureTarget = RenderTarget;
}

// Called when the game starts or when spawned
void AMH_MinimapActor::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerCharacter(this , 0);
	if (Player)
	{
		MinimapCameraBoom->AttachToComponent(Player->GetRootComponent() , FAttachmentTransformRules::KeepRelativeTransform);
		MinimapSprite->AttachToComponent(Player->GetRootComponent() , FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (MinimapRenderTargetClass)
	{
		MinimapCapture->TextureTarget = NewObject<UTextureRenderTarget2D>(this , MinimapRenderTargetClass);
	}

	if (MinimapSpriteClass)
	{
		MinimapSprite->SetSprite(NewObject<UPaperSprite>(this , MinimapSpriteClass));
	}

	//플레이어 메쉬 숨김
	MinimapCapture->ShowFlags.SetSkeletalMeshes(false);
	// 그림자도 숨김
	// 동적 그림자 비활성화
	MinimapCapture->ShowFlags.DynamicShadows = false;

	
}

// Called every frame
void AMH_MinimapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyMinimap();

}

void AMH_MinimapActor::ApplyMinimap()
{
	// 오너 캐릭터 확인 및 미니맵 업데이트
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this , 0);
	if (PlayerController)
	{
		Player = PlayerController->GetCharacter();
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
				CameraRotation = Player->GetControlRotation(); // 플레이어의 카메라(컨트롤러) 회전 값
			}
			// 미니맵 캡처의 회전을 카메라의 회전 값에 맞춰 설정
			FRotator MinimapRotation = FRotator(-90.f , CameraRotation.Yaw , 0.f);
			MinimapCapture->SetWorldRotation(MinimapRotation);

			//플레이어의 회전 포인트 이미지에 적용
			FRotator MinimapSpriteRotation = FRotator(
				FRotator::MakeFromEuler(FVector(90.f , 0.f , PlayerRotation.Yaw - 90.f)));
			MinimapSprite->SetWorldRotation(MinimapSpriteRotation);
		}
	}
}

