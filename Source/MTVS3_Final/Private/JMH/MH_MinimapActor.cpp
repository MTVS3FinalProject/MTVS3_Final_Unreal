// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_MinimapActor.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "JMH/MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_MinimapWidget.h"


// Sets default values
AMH_MinimapActor::AMH_MinimapActor()
{
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
	MinimapCapture->bEnableClipPlane = true;

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
	
	// // 스타일라운지
	// StyleLoungeSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("StyleLoungeSprite"));
	// if (StyleLoungeSprite)
	// {
	// 	StyleLoungeSprite->SetWorldLocation(FVector(18000, 4900, 3300)); // 티켓 커스터마이징 테이블 위치
	// 	StyleLoungeSprite->SetVisibility(true);
	// 	StyleLoungeSprite->SetWorldScale3D(FVector(2));
	// 	StyleLoungeSprite->bVisibleInSceneCaptureOnly = true;
	// }
}

void AMH_MinimapActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Player)
	{
		USpringArmComponent* PlayerCameraBoom = Player->FindComponentByClass<USpringArmComponent>();
		//MinimapCameraBoom->AttachToComponent(PlayerCameraBoom, FAttachmentTransformRules::KeepRelativeTransform);
		MinimapSprite->AttachToComponent(PlayerCameraBoom, FAttachmentTransformRules::KeepRelativeTransform);
	}

	if (MinimapRenderTargetClass)
	{
		MinimapCapture->TextureTarget = NewObject<UTextureRenderTarget2D>(this , MinimapRenderTargetClass);
	}
	
	if (MinimapSpriteClass)
	{
		MinimapSprite->SetSprite(NewObject<UPaperSprite>(this , MinimapSpriteClass));
		StyleLoungeSprite->SetSprite(NewObject<UPaperSprite>(this, MinimapSpriteClass));
	}

	//플레이어 메쉬 숨김
	MinimapCapture->ShowFlags.SetSkeletalMeshes(false);
	// 그림자도 숨김
	// 동적 그림자 비활성화
	MinimapCapture->ShowFlags.DynamicShadows = false;
	//지붕 없애기
	if (MinimapCapture)
	{
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

		for (AActor* Actor : AllActors)
		{
			// GetComponents의 결과를 TArray로 변환
			TSet<UActorComponent*> ComponentSet = Actor->GetComponents();
			TArray<UActorComponent*> Components;
			Components.Append(ComponentSet.Array()); // TSet을 TArray로 변환

			for (UActorComponent* Component : Components)
			{
				UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Component);
				if (MeshComponent && MeshComponent->ComponentHasTag(FName("HideFromMinimap")))
				{
					MinimapCapture->HiddenComponents.Add(MeshComponent);
					UE_LOG(LogTemp, Log, TEXT("Hidden component added: %s"), *MeshComponent->GetName());
				}
			}
		}
	}
}

void AMH_MinimapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyMinimap();
}

void AMH_MinimapActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MinimapCameraBoom)
	{
		MinimapCameraBoom->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}

	if (MinimapSprite)
	{
		MinimapSprite->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void AMH_MinimapActor::ApplyMinimap()
{
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

		// // StyleLoungeSprite 회전을 카메라 회전값에 적용하여 항상 똑바로 보이도록 설정
		// FRotator StyleLoungeRotation = FRotator(0, CameraRotation.Yaw - 90, 90);
		// StyleLoungeSprite->SetWorldRotation(StyleLoungeRotation);
		//
		// // 거리 기반 계산
		// FVector StyleLoungeLocation = StyleLoungeSprite->GetComponentLocation();
		// float Distance = FVector::Dist2D(PlayerLocation, StyleLoungeLocation);
		//
		// // 미니맵의 반경 (텍스처 경계)
		// float MinimapRadius = MinimapCapture->OrthoWidth / 2.0f;
		//
		// if (Distance > MinimapRadius) // 범위를 초과했을 경우 위치 보정
		// {
		// 	FVector Direction = (StyleLoungeLocation - PlayerLocation).GetSafeNormal();
		// 	FVector AdjustedLocation = PlayerLocation + Direction * (MinimapRadius - 50.0f);
		// 	StyleLoungeSprite->SetWorldLocation(FVector(AdjustedLocation.X, AdjustedLocation.Y, StyleLoungeLocation.Z));
		// }
		// else // 범위 안에 있으면 원래 위치 유지
		// {
		// 	StyleLoungeSprite->SetWorldLocation(FVector(18000, 4900, 3300));
		// }
	}
}

void AMH_MinimapActor::InitializeMinimap(ACharacter* LocalPlayer)
{
	//비긴플레이에서 플레이어 지정
	Player = LocalPlayer;
}