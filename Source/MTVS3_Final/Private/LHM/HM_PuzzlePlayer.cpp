// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"

#include "KismetTraceUtils.h"
#include "MovieSceneTracksComponentTypes.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	//HandComp->SetupAttachment(GetMesh(), TEXT("PiecePosition"));
	//HandComp->SetRelativeLocationAndRotation(FVector(-50, 0, 0), FRotator(0,90,-90));
	HandComp->SetupAttachment(FPSCameraComp);
	HandComp->SetRelativeLocation(FVector(120 , 0 , -40)); //(X=120.000000,Y=0.000000,Z=-40.000000)
	// (X=100.000000,Y=0.000000,Z=0.000000)
	// (Pitch=0.000000,Yaw=90.000000,Roll=-90.000000)
}

// Called when the game starts or when spawned
void AHM_PuzzlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if(GetLocalRole() == ROLE_Authority)
	{
		//for(int32 i = 1; i <= 9; i++)
		//{
			//태어날 때 모든 피스 목록을 기억하고 싶다.
		//	FName tag = FName(*FString::Printf(TEXT("Piece%d"), i));

			FName tag = TEXT("Piece");
			// 임시 AActor 배열에 피스 조각 수집
			TArray<AActor*> TempPieceList;
			UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AHM_PuzzlePiece::StaticClass() , tag ,
			                                             TempPieceList);
			
			for (AActor* actor : TempPieceList)
			{
				AHM_PuzzlePiece* piece = Cast<AHM_PuzzlePiece>(actor);
				if (piece)
				{
					// AHM_PuzzlePiece에 있는 모든 컴포넌트 가져오기
					//TArray<UActorComponent*> components = piece->GetComponentsByTag(UPrimitiveComponent::StaticClass(), tag);
					//if(components.Num() > 0)
					//{
						PieceList.Add(piece);
					//}
				}
			}
		//}
	}

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
				
				if (FPSCameraComp)
				{
					FPSCameraComp->SetWorldRotation(ControlRotation);
				}
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
		input->BindAction(IA_Pickup , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionPickupPiece);
		input->BindAction(IA_Launch , ETriggerEvent:: , this , &AHM_PuzzlePlayer::OnMyActionZoomInPiece);
		input->BindAction(IA_Launch , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionZoomOutPiece);
	}
		// 마우스 회전 입력 바인딩 추가
		PlayerInputComponent->BindAxis("Look Up", this, &AHM_PuzzlePlayer::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("Turn Right", this, &AHM_PuzzlePlayer::AddControllerYawInput);
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
	if (!bIsThirdPerson || bIsEnableLook)
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

				if(UStaticMeshComponent* PieceMesh = PickupPieceActor->GetPiece())
				{
					//FRotator PieceRotation = FRotator(ControlRotation.Pitch, 0, 0.0f);
					//PieceMesh->SetRelativeRotation(PieceRotation);
					//FRotator HandCompRotation = FRotator(ControlRotation.Pitch, 0, 0);
					//HandComp->SetRelativeRotation(HandCompRotation);
					//ServerRPCUpdatePieceRotation(HandCompRotation);
				}
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
	UE_LOG(LogTemp, Warning, TEXT("Pickup Action Triggered"));

	if (bIsZoomingIn && bHasPiece)
	{
		MyLaunchPiece();
		if(AimingUI) AimingUI->SetVisibility(ESlateVisibility::Hidden);
		//bHasPiece = false; 
		bIsZoomingIn = false;
	}
	else if(bHasPiece)
	{
		MyReleasePiece();
		bHasPiece = false;
	}
	else if(!bIsZoomingIn && !bHasPiece)
	{
		MyTakePiece();
		bHasPiece = true;
	}
}

void AHM_PuzzlePlayer::OnMyActionZoomInPiece(const FInputActionValue& Value)
{
	if ( bHasPiece && !bIsZoomingIn )
	{
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Visible);
		}
		ZoomIn();
	}
}

void AHM_PuzzlePlayer::OnMyActionZoomOutPiece(const FInputActionValue& Value)
{
	if ( bHasPiece && bIsZoomingIn )
	{
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Hidden);
		}
		ZoomOut();
	}
}

void AHM_PuzzlePlayer::MyTakePiece()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
				FString::Printf(TEXT("MyTakePiece")));

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector2D MousePosition;
	if (PC->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("클라이언트 겟마우스포지션 라인트레이스 실행"));
		// 마우스 위치를 월드 위치(Start)와 방향(Direction)으로 변환
		FVector Start, Dir;
		PC->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, Start, Dir);

		// 트레이스의 끝 위치 설정: Start 위치에서 Direction 방향으로 지정한 거리만큼 이동
		float TraceDistance = 1000.0f; // 예시 거리 (1,000 유닛)
		FVector End = Start + (Dir * TraceDistance);

		// 라인 트레이스 설정
		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("MouseTrace")), true, this);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.AddIgnoredActor(this); // 플레이어 자신을 무시

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult , Start , End , ECC_Visibility , TraceParams);

		// 라인 트레이스 디버그 라인
		DrawDebugLine(GetWorld() , Start , End , FColor::Blue , false , 5.f , 0 , 1.f);

		// 맞은 액터가 "Piece" 태그를 가진 AHM_PuzzlePiece인지 확인
		if (bHit && HitResult.GetActor())
		{
			AHM_PuzzlePiece* HitPiece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
			if (HitPiece && HitResult.Component->ComponentHasTag("Piece") && HitPiece->GetOwner() == nullptr)
			{
				ServerRPCTakePiece(HitPiece);
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
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
			FString::Printf(TEXT("MyReleasePiece")));
		ServerRPCReleasePiece();
	}
}

void AHM_PuzzlePlayer::MyLaunchPiece()
{
	if ( false == bHasPiece || false == IsLocallyControlled() )
		return;
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
			FString::Printf(TEXT("MyLaunchPiece")));
		ServerRPCLaunchPiece();
	}
}

void AHM_PuzzlePlayer::AttachPiece(AHM_PuzzlePiece* pieceActor)
{
	SwitchCamera(false);
	PickupPieceActor = pieceActor;

	// 클라이언트와 서버 모두에서 실행될 회전 로직
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		FRotator ControlRotation = PC->GetControlRotation();
        
		// 캐릭터의 회전을 카메라 회전과 동기화
		SetActorRotation(FRotator(0.0f, ControlRotation.Yaw, 0.0f));
        
		// 시점 전환을 부드럽게
		PC->SetViewTargetWithBlend(this);

	}
	
	if(PickupPieceActor && PickupPieceActor->GetPiece())
	{
		auto* mesh = PickupPieceActor->GetPiece();
		if(mesh)
		{
			mesh->SetSimulatePhysics(false); // 물리 비활성화
			mesh->SetEnableGravity(false);  // 중력 비활성화
			mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 비활성화

			if (PC && !bIsThirdPerson)
			{
				FRotator ControlRotation = PC->GetControlRotation(); //(Pitch=0.000000,Yaw=270.000000,Roll=-0.000000)
				//FRotator PieceRotation = FRotator(ControlRotation.Pitch, 0, 0); //(Pitch=-0.000000,Yaw=90.000000,Roll=-90.000000)
				FRotator HandCompRotation = FRotator(ControlRotation.Pitch, 0, 0);
				mesh->AttachToComponent(HandComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				mesh->SetRelativeLocation(FVector(100, 0, -30));
				//mesh->SetRelativeRotation(PieceRotation);
				//HandComp->SetRelativeRotation(HandCompRotation);
				UE_LOG(LogTemp, Log, TEXT("MulticastRPCTakePiece AttachPiece") );
			}
		}
	}
}

void AHM_PuzzlePlayer::DetachPiece(AHM_PuzzlePiece* pieceActor)
{
	SwitchCamera(true);
	if (pieceActor && pieceActor->GetPiece())
	{
		auto* mesh = pieceActor->GetPiece();
		check(mesh);
		if(mesh)
		{
			// 서버에서만 물리 시뮬레이션을 활성화
			//if (HasAuthority())
			//{
				mesh->SetSimulatePhysics(true);  // 물리 활성화
				mesh->SetEnableGravity(true);  // 중력 활성화
				pieceActor->bSimulatingPhysics = true;
			//}
			mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  // 충돌 활성화
			mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			// 현재 Transform을 저장하고 복제
			if(HasAuthority())
			{
				pieceActor->CurrentTransform = mesh->GetComponentTransform();
			}
		}
	}
}

void AHM_PuzzlePlayer::LaunchPiece(AHM_PuzzlePiece* pieceActor)
{
	SwitchCamera(true);
	if (pieceActor && pieceActor->GetPiece())
	{
		auto* mesh = pieceActor->GetPiece();
		check(mesh);
		if(mesh)
		{
			// 충돌 활성화
			mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			mesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
			mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			
			// 발사 속도를 적용하여 포물선 궤적 생성
			pieceActor->bSimulatingPhysics = true;
			mesh->SetSimulatePhysics(true); // 물리 적용
			mesh->SetEnableGravity(true);  // 중력 활성화
			mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

			//위치와 속도 설정
			FVector LaunchDirection = GetActorForwardVector(); // 캐릭터의 전방향
			//FVector FPSCompLocation = FPSCameraComp->GetComponentLocation().ForwardVector;
			float LaunchSpeed = 2000.0f; // 발사 속도 조정
			FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
			mesh->AddImpulse(LaunchVelocity, NAME_None, true);
			
			// FVector CameraLocation = FPSCameraComp->GetComponentLocation();
			// FVector LaunchDirection = (CameraLocation - mesh->GetComponentLocation()).GetSafeNormal();
			// float LaunchSpeed = 2000.0f;
			// FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
			// mesh->AddImpulse(LaunchVelocity, NAME_None, true);
		}
		// 현재 Transform을 저장하고 복제
		if(HasAuthority())
		{
			pieceActor->CurrentTransform = mesh->GetComponentTransform();
		}
	}
}

void AHM_PuzzlePlayer::ZoomIn()
{
	if (FPSCameraComp && !GetWorld()->GetTimerManager().IsTimerActive(ZoomTimerHandle))
	{
		bIsZoomingIn = true;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView, ZoomedFOV, GetWorld()->GetDeltaSeconds(), ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView, ZoomedFOV, 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				bIsZoomingIn = true; // 줌 인 상태로 고정
			}
		}, 0.01f, true);
		UE_LOG(LogTemp, Log, TEXT("ZoomIn Started"));
	}
}

void AHM_PuzzlePlayer::ZoomOut()
{
	if (FPSCameraComp && !GetWorld()->GetTimerManager().IsTimerActive(ZoomTimerHandle))
	{
		bIsZoomingIn = false;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle, [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView, DefaultFOV, GetWorld()->GetDeltaSeconds(), ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView, DefaultFOV, 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				bIsZoomingIn = false; // 줌 아웃 상태로 고정
			}
		}, 0.01f, true);
		UE_LOG(LogTemp, Log, TEXT("ZoomOut Started"));
	}
}

void AHM_PuzzlePlayer::ServerRPCTakePiece_Implementation(AHM_PuzzlePiece* pieceActor)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerRPCTakePiece called on Server"));

	if (!HasAuthority() || !pieceActor || pieceActor->GetOwner() != nullptr) return;

	pieceActor->SetOwner(this);
	PickupPieceActor = pieceActor;
	bHasPiece = true;

	// 모든 클라이언트에 알림
	MulticastRPCTakePiece(pieceActor);
	
	//if(GetLocalRole() != ROLE_Authority) return; // 서버에서만 실행
	// if (HitPiece&& HitResult
	// .
	// Component->ComponentHasTag("Piece") && HitPiece->GetOwner() == nullptr
	// )
	// {
	// 	HitPiece->SetOwner(this);
	// 	PickupPieceActor = HitPiece;
	// 	bHasPiece = true;
	//
	// 	// 모든 클라이언트에 알림
	// 	MulticastRPCTakePiece(HitPiece);
	// }
}

void AHM_PuzzlePlayer::MulticastRPCTakePiece_Implementation(AHM_PuzzlePiece* pieceActor)
{
	// 피스 액터를 HandComp에 붙이고 싶다.
	AttachPiece(pieceActor);
	UE_LOG(LogTemp, Log, TEXT("MulticastRPCTakePiece") );
}

void AHM_PuzzlePlayer::ServerRPCReleasePiece_Implementation()
{
	// 피스를 이미 주운 상태 -> 놓고싶다.
	if(bHasPiece)
	{
		bHasPiece = false;
	}

	// 피스의 오너를 취소하고싶다.
	if(PickupPieceActor)
	{
		MulticastRPCReleasePiece(PickupPieceActor);

		PickupPieceActor->SetOwner(nullptr);
		// 피스를 잊고싶다.
		PickupPieceActor = nullptr;
	}
}

void AHM_PuzzlePlayer::MulticastRPCReleasePiece_Implementation(AHM_PuzzlePiece* pieceActor)
{
	if (pieceActor)
	{
		DetachPiece(pieceActor); // 피스 분리
	}
}

void AHM_PuzzlePlayer::ServerRPCLaunchPiece_Implementation()
{
	if(bHasPiece)
	{
		bHasPiece = false;
	}
	
	if(PickupPieceActor)
	{
		MulticastRPCLaunchPiece(PickupPieceActor);

		PickupPieceActor->SetOwner(nullptr);
		// 피스를 잊고싶다.
		PickupPieceActor = nullptr;
	}
}

void AHM_PuzzlePlayer::MulticastRPCLaunchPiece_Implementation(AHM_PuzzlePiece* pieceActor)
{
	if (pieceActor)
	{
		LaunchPiece(pieceActor);
	}
}

void AHM_PuzzlePlayer::ServerRPCUpdateRotation_Implementation(const FRotator& NewRotation)
{
	// 서버에서 캐릭터 회전 적용
	SetActorRotation(NewRotation);
}

void AHM_PuzzlePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 복제하려는 변수 추가 예시
	DOREPLIFETIME(AHM_PuzzlePlayer, bHasPiece);
	DOREPLIFETIME(AHM_PuzzlePlayer, PickupPieceActor);
}
