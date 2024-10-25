// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LHM/HM_PuzzlePiece.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values
AHM_PuzzlePlayer::AHM_PuzzlePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	
}

// Called when the game starts or when spawned
void AHM_PuzzlePlayer::BeginPlay()
{
	Super::BeginPlay();
	

	if (!IsLocallyControlled()) return;

	auto* pc = Cast<APlayerController>(Controller);
	if ( pc )
	{
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if ( subSys ) subSys->AddMappingContext(IMC_PuzzlePlayer , 0);

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false); // 클릭 시 커서가 사라지지 않도록 설정
		pc->SetInputMode(InputMode);
		pc->bShowMouseCursor = true; // 마우스 커서 표시
	}
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	bIsHoldingPiece = false;
	bIsInTriggerZone = false;
}

// Called every frame
void AHM_PuzzlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform ttt = FTransform(GetControlRotation());
	Direction = ttt.TransformVector(Direction);

	AddMovementInput(Direction , 1);
	Direction = FVector::ZeroVector;

	// Physics Handle의 위치 업데이트
	if (PhysicsHandle && bIsHoldingPiece)
	{
		// 플레이어 전방 위치 계산
		//FVector HoldLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f) + (GetActorUpVector() * 50.0f);
		// Physics Handle의 목표 위치/회전 업데이트
		//PhysicsHandle->SetTargetLocationAndRotation(HoldLocation, GetActorRotation());

		FVector HoldLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f) + (GetActorUpVector() * 50.0f);
		PickedUpPiece->SetActorLocation(HoldLocation);
	}
}

// Called to bind functionality to input
void AHM_PuzzlePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if ( input )
	{
		input->BindAction(IA_Move , ETriggerEvent::Triggered , this , &AHM_PuzzlePlayer::OnMyActionMove);
		input->BindAction(IA_Look , ETriggerEvent::Triggered , this , &AHM_PuzzlePlayer::OnMyActionLook);
		input->BindAction(IA_EnableLook , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionEnableLookStart);
		input->BindAction(IA_EnableLook , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionEnableLookComplete);
		input->BindAction(IA_Jump , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionJumpStart);
		input->BindAction(IA_Jump , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionJumpComplete);
		input->BindAction(IA_Run , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionRunStart);
		input->BindAction(IA_Run , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionRunComplete);
		input->BindAction(IA_Pickup , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionPickupPiece);
		input->BindAction(IA_Launch , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionLaunchPickedUpPiece);
	}
}

void AHM_PuzzlePlayer::OnMyActionMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	Direction.X = v.X;
	Direction.Y = v.Y;
	Direction.Normalize();
}

void AHM_PuzzlePlayer::OnMyActionEnableLookStart(const FInputActionValue& Value)
{
	bIsEnableLook = true;
}

void AHM_PuzzlePlayer::OnMyActionEnableLookComplete(const FInputActionValue& Value)
{
	bIsEnableLook = false;
}

void AHM_PuzzlePlayer::OnMyActionLook(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	if ( bIsEnableLook )
	{
		AddControllerPitchInput(-v.Y);
		AddControllerYawInput(v.X);
	}
}

void AHM_PuzzlePlayer::OnMyActionJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void AHM_PuzzlePlayer::OnMyActionJumpComplete(const FInputActionValue& Value)
{
	StopJumping();
}

void AHM_PuzzlePlayer::OnMyActionRunStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AHM_PuzzlePlayer::OnMyActionRunComplete(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AHM_PuzzlePlayer::OnMyActionPickupPiece(const FInputActionValue& Value)
{
	PickupPiece();
	// if (!bIsHoldingPiece)
	// {
	// 	PickupPiece();
	// 	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("!bIsHoldingPiece / PickupPiece();")));
	// }
	// else if (bIsHoldingPiece)
	// {
	// 	DropPiece();
	// 	GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("bIsHoldingPiece / DropPiece();")));
	// }
}

void AHM_PuzzlePlayer::OnMyActionLaunchPickedUpPiece(const FInputActionValue& Value)
{
	LaunchPickedUpPiece();
}

void AHM_PuzzlePlayer::PickupPiece()
{
	// 플레이어의 현재 위치를 중심으로 스피어 트레이스
	FVector PlayerLocation = GetActorLocation();
	float SphereRadius = 120.0f; // 감지 범위 100 단위

	// 스피어 트레이스로 조각 감지
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("SpherePickupTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	// 스피어 트레이스 실행
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		PlayerLocation,          // 시작 위치
		PlayerLocation,          // 끝 위치 (같은 위치로 설정하여 순수 구체 체크)
		FQuat::Identity,        // 회전 없음
		ECC_Visibility,         // 채널
		FCollisionShape::MakeSphere(SphereRadius), // 구체 모양
		TraceParams
		);
		
	// 디버그 구체 표시
	DrawDebugSphere(
		GetWorld(),
		PlayerLocation,
		SphereRadius,
		32,                     // 세그먼트 수
		bHit ? FColor::Green : FColor::Red,
		false,                  // 지속성 없음
		1.0f,                   // 표시 시간
		0,                      // 우선순위
		1.0f                    // 두께
	);
	
	if(bHit)
	{
		AHM_PuzzlePiece* Piece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
		if (Piece && PhysicsHandle)
		{
			// 물리 핸들링 설정
			UPrimitiveComponent* PieceComponent = Piece->GetPiece();
			if (PieceComponent)
			{
				// 물리 시뮬레이션 비활성화
				PieceComponent->SetSimulatePhysics(true);
				PieceComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				
				// // Physics Handle을 사용하여 컴포넌트 제약
				// PhysicsHandle->GrabComponentAtLocationWithRotation(
				// 	PieceComponent ,
				// 	NAME_None ,
				// 	PieceComponent->GetComponentLocation() ,
				// 	PieceComponent->GetComponentRotation()
				// );

				// 상태 업데이트
				PickedUpPiece = Piece;
				bIsHoldingPiece = true;

				// 디버그 메시지
				UE_LOG(LogTemp , Log , TEXT("Picked Up Block: %s") , *Piece->GetName());
				GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("Pick up Piece!")));
			}
		}
	}
}

void AHM_PuzzlePlayer::DropPiece()
{
	if (PickedUpPiece)
	{
		UPrimitiveComponent* PieceComponent = PickedUpPiece->GetPiece();
		if (PieceComponent)
		{
			// 물리 시뮬레이션과 충돌을 다시 활성화
			PieceComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PieceComponent->SetSimulatePhysics(true);

			// 상태 초기화
			PickedUpPiece = nullptr;
			bIsHoldingPiece = false;

			UE_LOG(LogTemp, Log, TEXT("Piece Dropped"));
		}
	}
}

void AHM_PuzzlePlayer::LaunchPickedUpPiece()
{
	// 트리거 존에 있고 피스를 들고 있을 때만 발사 가능
	if (bIsInTriggerZone && PhysicsHandle && bIsHoldingPiece && PickedUpPiece)
	{
		LaunchPiece();
	}
	if (!bIsHoldingPiece) // 피스를 들고 있지 않을 때
	{
		TryPickupPiece();
	}
}

void AHM_PuzzlePlayer::LaunchPiece()
{
	//PhysicsHandle->ReleaseComponent();

	// 물리 시뮬레이션 다시 활성화
	UPrimitiveComponent* Piece = PickedUpPiece->GetPiece();
	if(Piece)
	{
		// 물리 시뮬레이션 활성화
		Piece->SetSimulatePhysics(true);
		Piece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            
		// 블럭의 앞방향으로 날림
		FVector LaunchDirection = GetActorForwardVector();
		//FVector Impulse = LaunchDirection * 1000.0f;
		//Piece->AddImpulse(Impulse, NAME_None, true);

		FVector LaunchVelocity = LaunchDirection * 1000.0f;
		Piece->SetPhysicsLinearVelocity(LaunchVelocity, true);
		
		// 약간의 회전도 추가
		// FVector RandomRotation(FMath::RandRange(-1.0f, 1.0f), 
		// 					 FMath::RandRange(-1.0f, 1.0f), 
		// 					 FMath::RandRange(-1.0f, 1.0f));
		// Piece->AddAngularImpulseInDegrees(RandomRotation * 100.0f, NAME_None, true);
		
		PickedUpPiece = nullptr;
		bIsHoldingPiece = false;

		// 디버그 메시지
		GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Yellow ,
		                                 FString::Printf(TEXT("Piece Launched!")));
	}
}

void AHM_PuzzlePlayer::TryPickupPiece()
{
	// 스피어 트레이스로 새로운 피스 찾기
        FHitResult HitResult;
        FCollisionQueryParams TraceParams(FName(TEXT("SpherePickupTrace")), true, this);
        TraceParams.bTraceComplex = true;
        TraceParams.bReturnPhysicalMaterial = false;

        // 플레이어의 현재 위치
        FVector PlayerLocation = GetActorLocation();
        float SphereRadius = 120.0f;

        // 스피어 트레이스 실행
        bool bHit = GetWorld()->SweepSingleByChannel(
            HitResult,
            PlayerLocation,
            PlayerLocation,
            FQuat::Identity,
            ECC_Visibility,
            FCollisionShape::MakeSphere(SphereRadius),
            TraceParams
        );

        // 디버그 스피어 표시
        DrawDebugSphere(
            GetWorld(),
            PlayerLocation,
            SphereRadius,
            32,
            bHit ? FColor::Green : FColor::Red,
            false,
            1.0f,
            0,
            1.0f
        );

        if (bHit)
        {
            AHM_PuzzlePiece* Piece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
            if (Piece && PhysicsHandle)
            {
                // 물리 핸들링 설정
                UPrimitiveComponent* PieceComponent = Piece->GetPiece();
                if (PieceComponent)
                {
                    // 물리 시뮬레이션 비활성화
                    PieceComponent->SetSimulatePhysics(false);
                	PieceComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 충돌은 쿼리만 가능하도록

                    // // Physics Handle을 사용하여 컴포넌트 제약
                    // PhysicsHandle->GrabComponentAtLocationWithRotation(
                    //     PieceComponent,
                    //     NAME_None,
                    //     PieceComponent->GetComponentLocation(),
                    //     PieceComponent->GetComponentRotation()
                    // );

                    // 상태 업데이트
                    PickedUpPiece = Piece;
                    bIsHoldingPiece = true;

                    // 디버그 메시지
                    UE_LOG(LogTemp, Log, TEXT("Picked Up Piece: %s"), *Piece->GetName());
                    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, 
                        FString::Printf(TEXT("Pick up Piece!")));
                }
            }
        }
}
