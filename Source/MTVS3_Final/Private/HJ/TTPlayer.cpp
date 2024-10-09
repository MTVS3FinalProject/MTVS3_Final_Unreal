// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATTPlayer::ATTPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0 , 0 , 70));
	SpringArmComp->TargetArmLength = 430;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ATTPlayer::BeginPlay()
{
	Super::BeginPlay();

	auto* pc = Cast<APlayerController>(Controller);
	if ( pc )
	{
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if ( subSys ) subSys->AddMappingContext(IMC_TTPlayer , 0);

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false); // 클릭 시 커서가 사라지지 않도록 설정
		pc->SetInputMode(InputMode);
		pc->bShowMouseCursor = true; // 마우스 커서 표시
	}
}

// Called every frame
void ATTPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform ttt = FTransform(GetControlRotation());
	Direction = ttt.TransformVector(Direction);

	AddMovementInput(Direction , 1);
	Direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ATTPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if ( input )
	{
		input->BindAction(IA_Move , ETriggerEvent::Triggered , this , &ATTPlayer::OnMyActionMove);
		input->BindAction(IA_EnableLook , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionEnableLookStart);
		input->BindAction(IA_EnableLook , ETriggerEvent::Completed , this , &ATTPlayer::OnMyActionEnableLookComplete);
		input->BindAction(IA_Look , ETriggerEvent::Triggered , this , &ATTPlayer::OnMyActionLook);
		input->BindAction(IA_Jump , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionJumpStart);
		input->BindAction(IA_Jump , ETriggerEvent::Completed , this , &ATTPlayer::OnMyActionJumpComplete);
		input->BindAction(IA_Dash , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionDashStart);
		input->BindAction(IA_Dash , ETriggerEvent::Completed , this , &ATTPlayer::OnMyActionDashComplete);
		input->BindAction(IA_Interact , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionInteract);
		input->BindAction(IA_Purchase , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionPurchase);
		input->BindAction(IA_Inventory , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionInventory);
	}
}

void ATTPlayer::OnMyActionMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	Direction.X = v.X;
	Direction.Y = v.Y;
	Direction.Normalize();
}

void ATTPlayer::OnMyActionEnableLookStart(const FInputActionValue& Value)
{
	bIsEnableLook = true;
}

void ATTPlayer::OnMyActionEnableLookComplete(const FInputActionValue& Value)
{
	bIsEnableLook = false;
}

void ATTPlayer::OnMyActionLook(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	if ( bIsEnableLook )
	{
		AddControllerPitchInput(-v.Y);
		AddControllerYawInput(v.X);
	}
}

void ATTPlayer::OnMyActionJumpStart(const FInputActionValue& Value)
{
	Jump();
}

void ATTPlayer::OnMyActionJumpComplete(const FInputActionValue& Value)
{
	StopJumping();
}

void ATTPlayer::OnMyActionDashStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 800;
}

void ATTPlayer::OnMyActionDashComplete(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 400;
}

void ATTPlayer::OnMyActionInteract(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed E: Interact"));
}

void ATTPlayer::OnMyActionPurchase(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed F: Purchase"));
}

void ATTPlayer::OnMyActionInventory(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed TAB: Inventory"));
}
