// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	HandComp->SetupAttachment(GetMesh(), TEXT("PiecePosition"));
	HandComp->SetRelativeLocationAndRotation(FVector(0, 60, 130), FRotator(0));
}

// Called when the game starts or when spawned
void AHM_PuzzlePlayer::BeginPlay()
{
	Super::BeginPlay();

	//SpawnPuzzlePieces();
	
	if(GetLocalRole() == ROLE_Authority)
	{
		//for(int32 i = 1; i <= 9; i++)
		//{
			// 태어날 때 모든 피스 목록을 기억하고 싶다.
			//FName tag = FName(*FString::Printf(TEXT("Piece%d"), i));

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
					PieceList.Add(piece);
				}
			}
		//}
	}

	//if (!IsLocallyControlled()) return;
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
}

// Called every frame
void AHM_PuzzlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bHasPiece)
	{
		FTransform ttt = FTransform(GetControlRotation());
		Direction = ttt.TransformVector(Direction);

		AddMovementInput(Direction , 1);
		Direction = FVector::ZeroVector;
	}
	else if(bHasPiece)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC && PC->IsLocalController())
		{
			FRotator ControlRotation = PC->GetControlRotation();

			if (!bIsThirdPerson) // If in first-person mode
			{
				// Set the actor's rotation to match the camera's rotation
				SetActorRotation(ControlRotation);
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
}

void AHM_PuzzlePlayer::SwitchCamera(bool _bIsThirdPerson)
{
	bIsThirdPerson = _bIsThirdPerson;
	
	if ( bIsThirdPerson ) // 3인칭 모드
	{
		GetMesh()->SetOwnerNoSee(false);
		FPSCameraComp->SetActive(false);
		TPSCameraComp->SetActive(true);

		// 플레이어의 회전 방향과 카메라 정렬
		APlayerController* PC = Cast<APlayerController>(GetController());
		if ( PC && PC->IsLocalController() )
		{
			PC->SetViewTargetWithBlend(this);  // 부드러운 시점 전환
		}
	}
	else // 1인칭 모드
	{
		FPSCameraComp->SetActive(true);
		TPSCameraComp->SetActive(false);

		// 플레이어의 회전 방향과 카메라 정렬
		APlayerController* PC = Cast<APlayerController>(GetController());
		if ( PC && PC->IsLocalController() )
		{
			// 캐릭터의 메시를 1인칭 시점에서 보이지 않게 설정
			GetMesh()->SetOwnerNoSee(true);
			if(bHasPiece)
			{
				FRotator ControlRotation = PC->GetControlRotation();
				PC->SetControlRotation(ControlRotation);
				SetActorRotation(ControlRotation);
				PC->SetViewTargetWithBlend(this);
			}
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
	UE_LOG(LogTemp, Warning, TEXT("Pickup Action Triggered"));

	if (bHasPiece)
	{
		MyReleasePiece();
	}
	else
	{
		MyTakePiece();
	}
}

void AHM_PuzzlePlayer::OnMyActionLaunchPieceStart(const FInputActionValue& Value)
{
	if (bHasPiece)
	{
		
	}
}

void AHM_PuzzlePlayer::OnMyActionLaunchPieceComplete(const FInputActionValue& Value)
{
	
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

void AHM_PuzzlePlayer::SpawnPuzzlePieces()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FVector SpawnOrigin = FVector(0, 0, 100); // 생성 위치의 기준점
	float SpawnRadius = 1000.0f; // 랜덤 위치를 위한 반경
	int32 NumberOfPieces = 9; // 생성할 퍼즐 조각 개수

	for (int32 i = 0; i < NumberOfPieces; ++i)
	{
		// 지정한 반경 내에서 랜덤 위치를 생성
		FVector RandomLocation = SpawnOrigin + FVector(FMath::RandPointInCircle(SpawnRadius), 0.0f);

		// 필요에 따라 Z축 높이를 기준 위치의 높이로 고정
		RandomLocation.Z = SpawnOrigin.Z;

		// 스폰 파라미터 설정
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// 퍼즐 조각 스폰
		AHM_PuzzlePiece* NewPiece = World->SpawnActor<AHM_PuzzlePiece>(AHM_PuzzlePiece::StaticClass(), RandomLocation, FRotator::ZeroRotator, SpawnParams);

		if (NewPiece)
		{
			// 고유 태그 할당
			FName TagName = FName(FString::Printf(TEXT("Piece%d"), i + 1));
			NewPiece->Tags.Add(TagName);

			UE_LOG(LogTemp, Warning, TEXT("Spawned %s at location %s"), *TagName.ToString(), *RandomLocation.ToString());
		}
	}
}

void AHM_PuzzlePlayer::AttachPiece(AHM_PuzzlePiece* pieceActor)
{
	SwitchCamera(false);
	PickupPieceActor = pieceActor;
	if(PickupPieceActor && PickupPieceActor->GetPiece())
	{
		auto* mesh = PickupPieceActor->GetPiece();
		if(mesh)
		{
			mesh->SetSimulatePhysics(false); // 물리 비활성화
			mesh->SetEnableGravity(false);  // 중력 비활성화
			mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 비활성화
			mesh->AttachToComponent(HandComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			UE_LOG(LogTemp, Log, TEXT("MulticastRPCTakePiece AttachPiece") );
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
	
	// // 피스를 줍지 않은 상태 -> 잡고싶다.
	// // 피스 목록을 검사하고 싶다.
	// for(AHM_PuzzlePiece* piece : PieceList)
	// {
	// 	if(!piece) continue;
	// 	
	// 	// 나와 피스의 거리가 PickupDistance 이하라면
	// 	// 그 중에 소유자가 없는 피스라면
	// 	float tempDist = GetDistanceTo(piece);
	// 	
	// 	UE_LOG(LogTemp, Warning, TEXT("AHM_PuzzlePiece* piece : PieceList"));
	// 	
	// 	if(tempDist <= PickupDistance && piece->GetOwner() == nullptr)
	// 	{
	// 		piece->SetOwner(this);  // Assign ownership on the server
	// 		PickupPieceActor = piece;
	// 		bHasPiece = true;
	//
	// 		UE_LOG(LogTemp, Warning, TEXT("Piece picked up on server by %s"), *GetName());
	//
	// 		MulticastRPCTakePiece(piece);  // Broadcast to all clients
	//
	// 		break;
	// 	}
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

void AHM_PuzzlePlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 복제하려는 변수 추가 예시
	DOREPLIFETIME(AHM_PuzzlePlayer, bHasPiece);
	DOREPLIFETIME(AHM_PuzzlePlayer, PickupPieceActor);

}
