// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzlePlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LHM/HM_PuzzlePiece.h"

// Sets default values
AHM_PuzzlePlayer::AHM_PuzzlePlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AHM_PuzzlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if ( !IsLocallyControlled() ) return;

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
}

// Called every frame
void AHM_PuzzlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform ttt = FTransform(GetControlRotation());
	Direction = ttt.TransformVector(Direction);

	AddMovementInput(Direction , 1);
	Direction = FVector::ZeroVector;


	if ( bIsHoldingPiece && PickedUpPiece )
	{
		// 플레이어 앞에 퍼즐 조각을 위치시킴 (예: 손의 위치 앞쪽에 고정)
		FVector NewLocation = GetActorLocation() + GetActorForwardVector() * 100.0f; // 플레이어 앞쪽 100 유닛
		PickedUpPiece->SetActorLocation(NewLocation);
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
		input->BindAction(IA_Launch , ETriggerEvent::Completed , this , &AHM_PuzzlePlayer::OnMyActionLaunchPickedUpPiece);
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
}

void AHM_PuzzlePlayer::OnMyActionLaunchPickedUpPiece(const FInputActionValue& Value)
{
	LaunchPickedUpPiece();
}

void AHM_PuzzlePlayer::PickupPiece()
{
	FVector CameraLocation;
	FRotator CameraRotation;
	GetController()->GetPlayerViewPoint(CameraLocation , CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 1000.0f);

	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("BlockPickupTrace")) , true , this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult , TraceStart , TraceEnd , ECC_Visibility , TraceParams);

	// 디버그 라인 그리기
	FColor LineColor = bHit ? FColor::Green : FColor::Red; // 히트 여부에 따라 색상 변경
	DrawDebugLine(GetWorld() , TraceStart , TraceEnd , LineColor , false , 1.0f , 0 , 1.0f);

	if ( bHit )
	{
		AHM_PuzzlePiece* Piece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
		if ( Piece )
		{
			UE_LOG(LogTemp , Log , TEXT("Picked Up Block: %s") , *Piece->GetName());

			// 퍼즐 조각을 플레이어가 "들고" 있는 상태로 만듭니다.
			PickedUpPiece = Piece;

			// 물리 상호작용 비활성화
			Piece->GetPiece()->SetSimulatePhysics(false);

			// 조각을 따라오게 함
			bIsHoldingPiece = true;
		}
	}
}

void AHM_PuzzlePlayer::LaunchPickedUpPiece()
{
	if ( PickedUpPiece )
	{
		// 블럭의 물리 상호작용을 다시 활성화
		PickedUpPiece->GetPiece()->SetSimulatePhysics(true);

		// 블럭의 앞방향 (Forward Vector)으로 날림
		FVector LaunchDirection = GetActorForwardVector(); // 플레이어의 앞방향
		FVector Impulse = LaunchDirection * 1000.0f; // 임펄스 크기

		PickedUpPiece->GetPiece()->AddImpulse(Impulse , NAME_None , true);

		// 블럭을 놓았으므로 참조 해제
		PickedUpPiece = nullptr;

		// 플레이어가 블럭을 들고 있지 않음을 표시
		bIsHoldingPiece = false;
	}
}
