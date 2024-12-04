// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/CameraPawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Spring Arm 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera 설정
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_Move , ETriggerEvent::Triggered , this , &ACameraPawn::OnMyActionMove);
	}
}

void ACameraPawn::OnMyActionMove(const FInputActionValue& Value)
{
	if (Controller)
	{
		const FVector2D InputValue = Value.Get<FVector2D>();
        
		// 현재 카메라의 회전값 가져오기
		const FRotator CameraRotation = Controller->GetControlRotation();
        
		// 좌우 이동: 카메라의 Right 벡터 기준으로 이동
		if (InputValue.X != 0.0f)
		{
			const FVector RightDirection = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
			AddMovementInput(RightDirection, InputValue.X);
		}
        
		// 상하 이동: World Up 벡터 기준으로 이동
		if (InputValue.Y != 0.0f)
		{
			AddMovementInput(FVector::UpVector, InputValue.Y);
		}
	}
}

