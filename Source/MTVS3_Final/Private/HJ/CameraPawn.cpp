// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/CameraPawn.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HJ/TTPlayer.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 빈 루트 컴포넌트 생성 (피봇 포인트로 사용)
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));

	// Spring Arm 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = OrbitRadius;  // 피봇에서 카메라까지의 거리
	SpringArmComp->bUsePawnControlRotation = true;
    
	// Camera 설정
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	// 네트워크 설정
	NetUpdateFrequency = 30.0f;
	MinNetUpdateFrequency = 10.0f;
}

void ACameraPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACameraPawn, OriginalPlayer);
	DOREPLIFETIME(ACameraPawn, NetworkedTransform);
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
		input->BindAction(IA_Interact , ETriggerEvent::Started , this , &ACameraPawn::OnMyActionInteract);
	}
}

void ACameraPawn::OnMyActionMove(const FInputActionValue& Value)
{
	if (!IsLocallyControlled()) return;

	const FVector2D InputValue = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	FVector NewLocation = GetActorLocation();

	// X 입력값(좌/우 키)을 사용해 좌우 이동
	if (InputValue.X != 0.0f)
	{
		const FVector RightDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		NewLocation += RightDirection * InputValue.X * MoveSpeed * GetWorld()->GetDeltaSeconds();
	}

	// Y 입력값(위/아래 키)을 사용해 상하 이동
	if (InputValue.Y != 0.0f)
	{
		NewLocation += FVector::UpVector * InputValue.Y * MoveSpeed * GetWorld()->GetDeltaSeconds();
	}

	FTransform NewTransform = GetActorTransform();
	NewTransform.SetLocation(NewLocation);

	// 로컬에서 즉시 적용
	UpdateCameraTransform(NewTransform);
    
	// 서버에 업데이트 요청
	ServerUpdateTransform(NewTransform);
}

void ACameraPawn::OnMyActionInteract(const FInputActionValue& Value)
{
	if (!IsLocallyControlled()) return;
    
	// GetOwner를 TTPlayer로 캐스팅
	ATTPlayer* Player = Cast<ATTPlayer>(GetOwner());
	if (Player)
	{
		Player->bIsInCameraMode = false;
		ServerReturnToOriginalPlayer();
	}
}

void ACameraPawn::OnRep_NetworkedTransform()
{
	if (!IsLocallyControlled())
	{
		UpdateCameraTransform(NetworkedTransform);
	}
}

void ACameraPawn::UpdateCameraTransform(const FTransform& NewTransform)
{
	SetActorTransform(NewTransform);
}

void ACameraPawn::SetOriginalPlayer(ATTPlayer* Player)
{
	OriginalPlayer = Player;
}

void ACameraPawn::ServerReturnToOriginalPlayer_Implementation()
{
	// GetOwner를 TTPlayer로 캐스팅
	ATTPlayer* Player = Cast<ATTPlayer>(GetOwner());
	if (!Player) return;

	// 플레이어의 ReturnFromCameraPawn 함수 호출
	Player->ServerReturnFromCameraPawn();

	// CameraPawn 제거
	Destroy();
}

void ACameraPawn::ServerUpdateTransform_Implementation(const FTransform& NewTransform)
{
	NetworkedTransform = NewTransform;
	UpdateCameraTransform(NetworkedTransform);
}

