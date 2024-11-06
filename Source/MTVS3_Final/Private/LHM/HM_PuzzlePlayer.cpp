// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "LHM/HM_AimingWidget.h"
#include "LHM/HM_PuzzlePiece.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHM_PuzzlePlayer::AHM_PuzzlePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonSpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0 , 0 , 50));
	SpringArmComp->TargetArmLength = 430;
	SpringArmComp->bUsePawnControlRotation = true;

	TPSCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	TPSCameraComp->SetupAttachment(SpringArmComp);
	TPSCameraComp->bUsePawnControlRotation = false;

	FPSCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComp->SetupAttachment(RootComponent);
	FPSCameraComp->SetRelativeLocation(FVector(-100 , 0 , 80));
	FPSCameraComp->bUsePawnControlRotation = true;
	
	HandComp = CreateDefaultSubobject<USceneComponent>(TEXT("HandComp"));
	HandComp->SetupAttachment(FPSCameraComp);
	HandComp->SetRelativeLocation(FVector(120 , 0 , -40));
}

// Called when the game starts or when spawned
void AHM_PuzzlePlayer::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocallyControlled())
	{
		auto* pc = Cast<APlayerController>(Controller);
		if (pc)
		{
			UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				pc->GetLocalPlayer());
			if (subSys) subSys->AddMappingContext(IMC_PuzzlePlayer , 0);

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false); // 클릭 시 커서가 사라지지 않도록 설정
			pc->SetInputMode(InputMode);
			pc->bShowMouseCursor = true; // 마우스 커서 표시
		}
	}
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bReplicates = true;
	
	AimingUI = Cast<UHM_AimingWidget>(CreateWidget(GetWorld() , AimingUIFactory));
	if ( AimingUI )
	{
		AimingUI->AddToViewport();
		AimingUI->SetVisibility(ESlateVisibility::Hidden);
	}

	if (FPSCameraComp)
	{
		DefaultFOV = FPSCameraComp->FieldOfView;
	}
}

// Called every frame
void AHM_PuzzlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->IsLocalController()) return;
	
	if(!bHasPiece)
	{
		FTransform ttt = FTransform(GetControlRotation());
		Direction = ttt.TransformVector(Direction);

		AddMovementInput(Direction , 1);
		Direction = FVector::ZeroVector;
	}
	else if(bHasPiece)
	{
		if (PC && PC->IsLocalController())
		{
			FRotator ControlRotation = PC->GetControlRotation();

			if (!bIsThirdPerson) // If in first-person mode
			{
				// Set the actor's rotation to match the camera's rotation
				FRotator NewRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);
				SetActorRotation(NewRotation);

				// 서버에 회전 값 전달
				ServerRPCUpdateRotation(NewRotation);
				ServerRPCUpdateFPSCameraRotation(ControlRotation);
			}

			// Calculate movement direction based on camera rotation
			Direction = FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::Y) * Direction.Y +
						FRotationMatrix(ControlRotation).GetUnitAxis(EAxis::X) * Direction.X;

			AddMovementInput(Direction);
			Direction = FVector::ZeroVector;
		}
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
		input->BindAction(IA_Piece , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionPickupPiece);
		input->BindAction(IA_Zoom , ETriggerEvent::Triggered , this , &AHM_PuzzlePlayer::OnMyActionZoomInPiece);
		input->BindAction(IA_Zoom , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionZoomOutPiece);
	}
		// 마우스 회전 입력 바인딩 추가
		//PlayerInputComponent->BindAxis("Look Up", this, &AHM_PuzzlePlayer::AddControllerPitchInput);
		//PlayerInputComponent->BindAxis("Turn Right", this, &AHM_PuzzlePlayer::AddControllerYawInput);
}

void AHM_PuzzlePlayer::SwitchCamera(bool _bIsThirdPerson)
{
	bIsThirdPerson = _bIsThirdPerson;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if(!PC || !PC->IsLocalController()) return;
	
	if ( bIsThirdPerson ) // 3인칭 모드
	{
		// 마우스 커서 보이기 및 게임 모드 설정
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
		
		GetMesh()->SetOwnerNoSee(false);
		FPSCameraComp->SetActive(false);
		TPSCameraComp->SetActive(true);
		PC->SetViewTargetWithBlend(this);  // 부드러운 시점 전환
	}
	else // 1인칭 모드
	{
		// 마우스 커서 숨기기 및 게임 모드 설정
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		
		FPSCameraComp->SetActive(true);
		TPSCameraComp->SetActive(false);
		GetMesh()->SetOwnerNoSee(true);
		
		if(bHasPiece)
		{
			FRotator ControlRotation = PC->GetControlRotation();
			SetActorRotation(FRotator(0, ControlRotation.Yaw, 0));
			PC->SetViewTargetWithBlend(this);
		}

		// 마우스 감도 설정
		if (APlayerCameraManager* CameraMgr = PC->PlayerCameraManager)
		{
			CameraMgr->ViewPitchMin = -50.0f;
			CameraMgr->ViewPitchMax = 50.0f;
		}
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
	
	// 1인칭 모드이거나 마우스 좌클릭(bIsEnableLook)일 때 시점 이동 가능
	if (!bIsThirdPerson || bIsEnableLook )
	{
		AddControllerPitchInput(-v.Y);
		AddControllerYawInput(v.X);
		
		// 1인칭 모드이고 피스를 들고 있을 때는 캐릭터도 회전
		if (!bIsThirdPerson && bHasPiece)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC && PC->IsLocalController())
			{
				FRotator ControlRotation = PC->GetControlRotation();
				FRotator NewRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);
				SetActorRotation(NewRotation);

				// 서버에 회전 값 전달
				ServerRPCUpdateRotation(NewRotation);
				ServerRPCUpdateFPSCameraRotation(ControlRotation);
			}
		}
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
	//UE_LOG(LogTemp, Warning, TEXT("Pickup Action Triggered"));

	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, 
	//    FString::Printf(TEXT("bIsZoomingIn: %s, bHasPiece: %s"), 
	//    bIsZoomingIn ? TEXT("True") : TEXT("False"), 
	//    bHasPiece ? TEXT("True") : TEXT("False")));

	if (bIsZoomingIn && bHasPiece)
	{
		MyLaunchPiece();
		if(AimingUI) AimingUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else if(bHasPiece)
	{
		MyReleasePiece();
	}
	//else if(!bIsZoomingIn && !bHasPiece)
	else if(!bHasPiece)
	{
		MyTakePiece();
	}
}

void AHM_PuzzlePlayer::OnMyActionZoomInPiece(const FInputActionValue& Value)
{
	if ( bHasPiece && !bIsZoomingIn && !bIsThirdPerson )
	{
		bIsZoomingIn = true;
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Visible);
		}
		ZoomIn();
	}
}

void AHM_PuzzlePlayer::OnMyActionZoomOutPiece(const FInputActionValue& Value)
{
	if ( bHasPiece && bIsZoomingIn && !bIsThirdPerson )
	{
		bIsZoomingIn = false;
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Hidden);
		}
		ZoomOut();
	}
}

void AHM_PuzzlePlayer::MyTakePiece()
{
	ZoomOut();
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
	// 			FString::Printf(TEXT("MyTakePiece")));

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector2D MousePosition;
	if (PC->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		// 마우스 위치를 월드 위치(Start)와 방향(Direction)으로 변환
		FVector Start, Dir;
		PC->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, Start, Dir);

		// 트레이스의 끝 위치 설정: Start 위치에서 Direction 방향으로 지정한 거리만큼 이동
		float TraceDistance = 1000.0f;
		FVector End = Start + (Dir * TraceDistance);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("MouseTrace")), true, this);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult , Start , End , ECC_Visibility , TraceParams);

		DrawDebugLine(GetWorld() , Start , End , FColor::Blue , false , 5.f , 0 , 1.f);

		// 맞은 액터가 "Piece" 태그를 가진 AHM_PuzzlePiece인지 확인
		if (bHit && HitResult.GetComponent())
		{
			AHM_PuzzlePiece* HitPiece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
			if(HitPiece)
			{
				UStaticMeshComponent* HitComponent = Cast<UStaticMeshComponent>(HitResult.GetComponent());
				if (HitComponent && HitPiece->GetAllPieces().Contains(HitComponent))
				{
					// 소유되지 않은 상태이며, 특정 태그가 있는지 확인
					if (!HitPiece->IsComponentOwned(HitComponent))
					{
						for (int32 i = 1; i <= 9; ++i)
						{
							FString TagName = FString::Printf(TEXT("Piece%d"), i);
							if (HitComponent->ComponentHasTag(FName(*TagName)))
							{
								//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange,
								//								 FString::Printf(TEXT("Found piece with tag: %s"), *TagName));
								TargetPieceComp = HitComponent;

								// 서버로 소유 요청을 보냄
								ServerRPCTakePiece(HitPiece, TargetPieceComp);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void AHM_PuzzlePlayer::MyReleasePiece()
{
	// 피스를 잡고 있지 않으면 피스를 놓을 수 없다.
	if ( false == bHasPiece || false == IsLocallyControlled() )
		return;
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
		//	FString::Printf(TEXT("MyReleasePiece")));
		ServerRPCReleasePiece();
	}
}

void AHM_PuzzlePlayer::MyLaunchPiece()
{
	if ( false == bHasPiece || false == IsLocallyControlled() )
		return;
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
		//	FString::Printf(TEXT("MyLaunchPiece")));
		ServerRPCLaunchPiece();
	}
}

void AHM_PuzzlePlayer::AttachPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCamera(false);
	
	// 클라이언트와 서버 모두에서 실행될 회전 로직
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		FRotator ControlRotation = PC->GetControlRotation();
		// 캐릭터의 회전을 카메라 회전과 동기화
		SetActorRotation(FRotator(0.0f, ControlRotation.Yaw, 0.0f));
		PC->SetViewTargetWithBlend(this);
	}
	
	if(PieceComp)
	{
		PieceComp->SetSimulatePhysics(false);
		PieceComp->SetEnableGravity(false);
		PieceComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// if(IsLocallyControlled())
		if (PC && !bIsThirdPerson)
		{
			PieceComp->AttachToComponent(HandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PieceComp->SetRelativeLocation(FVector(100 , 0 , -30));
		}
	}
	if (HasAuthority())
	{
		TargetPieceTransform = PieceComp->GetComponentTransform();
	}
}

void AHM_PuzzlePlayer::DetachPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCamera(true);
	
	PieceComp->SetSimulatePhysics(true);
	PieceComp->SetEnableGravity(true);
	PieceComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PieceComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if (HasAuthority())
	{
		TargetPieceTransform = PieceComp->GetComponentTransform();
	}
	TargetPieceComp = nullptr;
}

void AHM_PuzzlePlayer::LaunchPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCamera(true);

	PieceComp->SetSimulatePhysics(true);
	PieceComp->SetEnableGravity(true);
	PieceComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PieceComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	PieceComp->SetCollisionProfileName(TEXT("PuzzlePiece"));
	PieceComp->SetNotifyRigidBodyCollision(true);
	PieceComp->SetGenerateOverlapEvents(true);

	FVector LaunchDirection = FPSCameraComp->GetForwardVector();
	float LaunchSpeed = 4000.0f;
	FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
	PieceComp->AddImpulse(LaunchVelocity, NAME_None, true);

	if (HasAuthority())
	{
		TargetPieceTransform = PieceComp->GetComponentTransform();
	}
	TargetPieceComp = nullptr;
}

void AHM_PuzzlePlayer::ZoomIn()
{
	if (FPSCameraComp)
	{
		// 기존 타이머가 있다면 종료
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		
		bIsZoomingIn = true;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView, ZoomedFOV, GetWorld()->GetDeltaSeconds(), ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView, ZoomedFOV, 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				//bIsZoomingIn = true; // 줌 인 상태로 고정
			}
		}, 0.01f, true);
		//UE_LOG(LogTemp, Log, TEXT("ZoomIn Started"));
	}
}

void AHM_PuzzlePlayer::ZoomOut()
{
	if(FPSCameraComp)
	{
		// 기존 타이머가 있다면 종료
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		
		bIsZoomingIn = false;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView, DefaultFOV, GetWorld()->GetDeltaSeconds(), ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView, DefaultFOV, 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				//bIsZoomingIn = false; // 줌 아웃 상태로 고정
			}
		}, 0.01f, true);
		//UE_LOG(LogTemp, Log, TEXT("ZoomOut Completed"));
	}
}

void AHM_PuzzlePlayer::ServerRPCTakePiece_Implementation(AHM_PuzzlePiece* pieceActor, UStaticMeshComponent* PieceComp)
{
	if (!HasAuthority() || !pieceActor || !PieceComp || pieceActor->IsComponentOwned(PieceComp)) return;
	
	//pieceActor->SetComponentOwner(PieceComp,this);
	PickupPieceActor = pieceActor;
	TargetPieceComp = PieceComp;
	bHasPiece = true;

	// 모든 클라이언트에 알림
	MulticastRPCTakePiece(PieceComp);
}

void AHM_PuzzlePlayer::MulticastRPCTakePiece_Implementation(UStaticMeshComponent* PieceComp)
{
	AttachPiece(PieceComp);
}

void AHM_PuzzlePlayer::ServerRPCReleasePiece_Implementation()
{
	if (bHasPiece && TargetPieceComp)
	{
		bHasPiece = false;
		PickupPieceActor->ClearComponentOwner(TargetPieceComp);
		MulticastRPCReleasePiece(TargetPieceComp);
		PickupPieceActor = nullptr;
		TargetPieceComp = nullptr;
	}
}

void AHM_PuzzlePlayer::MulticastRPCReleasePiece_Implementation(UStaticMeshComponent* PieceComp)
{
	if (PieceComp)
	{
		DetachPiece(PieceComp); // 피스 분리
		TargetPieceComp = nullptr;
	}
}

void AHM_PuzzlePlayer::ServerRPCLaunchPiece_Implementation()
{
	if (bHasPiece && TargetPieceComp && PickupPieceActor)
	{
		// 피스의 마지막 소유자를 현재 소유자로 설정
		PickupPieceActor->LastOwners.Add(TargetPieceComp, this);
		
		bHasPiece = false;
		PickupPieceActor->ClearComponentOwner(TargetPieceComp);
		MulticastRPCLaunchPiece(TargetPieceComp);
		PickupPieceActor = nullptr;
		TargetPieceComp = nullptr;
	}
	bIsZoomingIn = false;
}

void AHM_PuzzlePlayer::MulticastRPCLaunchPiece_Implementation(UStaticMeshComponent* PieceComp)
{
	if (PieceComp)
	{
		LaunchPiece(PieceComp);
		TargetPieceComp = nullptr;
	}
}

void AHM_PuzzlePlayer::ServerRPCUpdateRotation_Implementation(const FRotator& NewRotation)
{
	// 서버에서 캐릭터 회전 적용
	SetActorRotation(NewRotation);
}

void AHM_PuzzlePlayer::ServerRPCUpdateFPSCameraRotation_Implementation(const FRotator& FPSCameraNewRotation)
{
	if(FPSCameraComp)
	{
		FPSCameraComp->SetWorldRotation(FPSCameraNewRotation);
	}
}

void AHM_PuzzlePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHM_PuzzlePlayer, bHasPiece);
	DOREPLIFETIME(AHM_PuzzlePlayer, bIsZoomingIn);
	DOREPLIFETIME(AHM_PuzzlePlayer, PickupPieceActor);
	DOREPLIFETIME(AHM_PuzzlePlayer, TargetPieceComp);
	DOREPLIFETIME(AHM_PuzzlePlayer, TargetPieceTransform);
}
