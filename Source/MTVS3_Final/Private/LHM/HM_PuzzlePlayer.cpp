// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"

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
		input->BindAction(IA_Launch , ETriggerEvent::Started , this , &AHM_PuzzlePlayer::OnMyActionLaunchPieceStart);
		input->BindAction(IA_Launch , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionLaunchPieceComplete);
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

	if (bHasPiece)
	{
		MyReleasePiece();
		UE_LOG(LogTemp, Warning, TEXT("MyReleasePiece"));
	}
	else
	{
		MyTakePiece();
		UE_LOG(LogTemp, Warning, TEXT("MyTakePiece"));
	}
}

void AHM_PuzzlePlayer::OnMyActionLaunchPieceStart(const FInputActionValue& Value)
{
	if (bHasPiece)
	{
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		return;
	}
}

void AHM_PuzzlePlayer::OnMyActionLaunchPieceComplete(const FInputActionValue& Value)
{
	if (bHasPiece)
	{
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Hidden);
			MyLaunchPiece();
		}
	}
	else
	{
		return;
	}
}

void AHM_PuzzlePlayer::MyTakePiece()
{
	ServerRPCTakePiece();
}

void AHM_PuzzlePlayer::MyReleasePiece()
{
	// 피스를 잡고 있지 않으면 피스를 놓을 수 없다.
	if ( false == bHasPiece || false == IsLocallyControlled() )
		return;
	
	ServerRPCReleasePiece();
}

void AHM_PuzzlePlayer::MyLaunchPiece()
{
	if ( false == bHasPiece || false == IsLocallyControlled() )
		return;
	
	ServerRPCLaunchPiece();
	
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

			// Piece를 손에 부착할 때 카메라의 Pitch 값도 적용
			//APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC && !bIsThirdPerson)
			{
				FRotator ControlRotation = PC->GetControlRotation(); //(Pitch=0.000000,Yaw=270.000000,Roll=-0.000000)
				//FRotator PieceRotation = FRotator(ControlRotation.Pitch, 0, 0); //(Pitch=-0.000000,Yaw=90.000000,Roll=-90.000000)
				FRotator HandCompRotation = FRotator(ControlRotation.Pitch, 0, 0);
				mesh->AttachToComponent(HandComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				mesh->SetRelativeLocation(FVector(100, 0, 0));
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
			// 발사 속도를 적용하여 포물선 궤적 생성
			mesh->SetSimulatePhysics(true); // 물리 적용
			mesh->SetEnableGravity(true);  // 중력 활성화
			pieceActor->bSimulatingPhysics = true;

			// 충돌 활성화
			mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			mesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
			mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

			// 위치와 속도 설정
			FVector LaunchDirection = GetActorForwardVector(); // 캐릭터의 전방향
			float LaunchSpeed = 1000.0f; // 발사 속도 조정
			FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
			mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			mesh->AddImpulse(LaunchVelocity, NAME_None, true);
		}
		// 현재 Transform을 저장하고 복제
		if(HasAuthority())
		{
			pieceActor->CurrentTransform = mesh->GetComponentTransform();
		}
	}
}

void AHM_PuzzlePlayer::ServerRPCTakePiece_Implementation()
{
	if(GetLocalRole() != ROLE_Authority) return; // 서버에서만 실행
	
	UE_LOG(LogTemp, Warning, TEXT("ServerRPCTakePiece called on Server"));

	AHM_PuzzlePiece* ClosestPiece = nullptr;
	float ClosestDistance = PickupDistance;

	for(AHM_PuzzlePiece* piece : PieceList)
	{
		if (!piece) continue;
        
		float tempDist = GetDistanceTo(piece);
        
		if(tempDist <= ClosestDistance && piece->GetOwner() == nullptr)
		{
			ClosestDistance = tempDist;
			ClosestPiece = piece;
		}
	}

	if (ClosestPiece)
	{
		ClosestPiece->SetOwner(this);
		PickupPieceActor = ClosestPiece;
		bHasPiece = true;

		// 모든 클라이언트에 알림
		MulticastRPCTakePiece(ClosestPiece);
	}
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
