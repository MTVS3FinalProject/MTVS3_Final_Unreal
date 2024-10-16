// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "JMH/MH_Chair.h"
#include "HJ/TTPlayerAnim.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"   // TActorIterator 정의 포함
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "JMH/MH_TicketingWidget.h"
#include "JMH/MainWidget.h"

// Sets default values
ATTPlayer::ATTPlayer()
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

	NicknameUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NicknameUI"));
	// 머리 본(Bone)에 부착
	NicknameUIComp->SetupAttachment(GetMesh() , TEXT("head"));  // "head" 본에 부착

	// 머리 위로 약간 올리기 위한 위치 조정
	NicknameUIComp->SetRelativeLocationAndRotation(FVector(30.0f , 0 , 0), FRotator(0, 90.0f, -90.0f));  // 머리 위로 30 단위 상승
	/*NicknameUIComp->SetupAttachment(RootComponent);
	NicknameUIComp->SetRelativeLocation(FVector(0 , 0 , 100));*/
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
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if ( UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("TTHallMap") || UGameplayStatics::GetCurrentLevelName(GetWorld()) == TEXT("KHJProtoMap") ) {
		// 특정 레벨일 때 실행할 코드
		UE_LOG(LogTemp , Warning , TEXT("현재 레벨은 TTHallMap입니다."));
		InitMainUI();
	}
	SwitchCamera(bIsThirdPerson);
}

// Called every frame
void ATTPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform ttt = FTransform(GetControlRotation());
	Direction = ttt.TransformVector(Direction);

	AddMovementInput(Direction , 1);
	Direction = FVector::ZeroVector;

	if ( NicknameUIComp && NicknameUIComp->GetVisibleFlag() )
	{
		// P = P0 + vt
		// 카메라 위치
		FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		// 체력바와 카메라의 방향 벡터
		FVector NicknameUIDirection = CamLoc - NicknameUIComp->GetComponentLocation();
		//NicknameUIDirection.Z = 0.0f;

		NicknameUIComp->SetWorldRotation(NicknameUIDirection.GetSafeNormal().ToOrientationRotator());
	}
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
		input->BindAction(IA_Run , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionRunStart);
		input->BindAction(IA_Run , ETriggerEvent::Completed , this , &ATTPlayer::OnMyActionRunComplete);
		input->BindAction(IA_Interact , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionInteract);
		input->BindAction(IA_Purchase , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionPurchase);
		input->BindAction(IA_Inventory , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionInventory);
		input->BindAction(IA_Chat , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionChat);
	}
}

void ATTPlayer::SwitchCamera(bool _bIsThirdPerson)
{
	if ( _bIsThirdPerson ) // 3인칭 모드
	{
		GetMesh()->SetOwnerNoSee(false);
		FPSCameraComp->SetActive(false);
		TPSCameraComp->SetActive(true);
		NicknameUIComp->SetOwnerNoSee(false);

		// 플레이어의 회전 방향과 카메라 정렬
		APlayerController* PC = Cast<APlayerController>(GetController());
		if ( PC )
		{
			PC->SetViewTargetWithBlend(this);  // 부드러운 시점 전환
		}
	}
	else // 1인칭 모드
	{
		FPSCameraComp->SetActive(true);
		TPSCameraComp->SetActive(false);
		NicknameUIComp->SetOwnerNoSee(true);

		// 플레이어의 회전 방향과 카메라 정렬
		APlayerController* PC = Cast<APlayerController>(GetController());
		if ( PC )
		{
			// 캐릭터의 메시를 1인칭 시점에서 보이지 않게 설정
			GetMesh()->SetOwnerNoSee(true);

			// 캐릭터의 현재 회전 방향으로 카메라를 맞춤
			FRotator ControlRotation = GetActorRotation();
			PC->SetControlRotation(ControlRotation);

			PC->SetViewTargetWithBlend(this);  // 부드러운 시점 전환
		}
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

void ATTPlayer::OnMyActionRunStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ATTPlayer::OnMyActionRunComplete(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATTPlayer::OnMyActionInteract(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed E: Interact"));
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	if ( Chair )
	{
		// 의자가 비어 있을 때 상호작용하면 앉는다.
		// 1인칭 시점으로 전환
		if ( !Chair->bIsOccupied )
		{
			UE_LOG(LogTemp , Warning , TEXT("Chair->bIsOccupied = true"));

			// MainUI 숨기기
			MainUI->SetVisibleCanvas(false);
			// 좌석 접수 UI 표시
			TicketingUI->SetVisibleSwitcher(true);
			TicketingUI->SetWidgetSwitcher(1);

			ServerSetSitting(true);

			// 의자의 회전값 가져오기
			FRotator ChairRotation = Chair->GetActorRotation();

			// 카메라의 회전을 의자에 맞게 설정
			FPSCameraComp->SetWorldRotation(ChairRotation);  // 의자 방향으로 카메라 회전 설정
			SwitchCamera(!bIsThirdPerson);

			// 15초 후에 자동으로 일어나도록 타이머 시작
			GetWorld()->GetTimerManager().SetTimer(
				StandUpTimerHandle , this , &ATTPlayer::ForceStandUp , MaxSittingDuration , false);
		}
		// 의자가 비어 있지 않고 내가 앉아 있으면 일어난다.
		// 3인칭 시점으로 전환
		else if ( Chair->bIsOccupied && bIsSitting )
		{
			UE_LOG(LogTemp , Warning , TEXT("Chair->bIsOccupied = false"));

			// MainUI 표시
			MainUI->SetVisibleCanvas(true);
			// 좌석 접수 UI 숨기기
			TicketingUI->SetVisibleSwitcher(false);

			ServerSetSitting(false);
			SwitchCamera(bIsThirdPerson);
		}
		// 의자가 비어 있지 않고 내가 앉아 있지 않으면 아무 일도 안 일어난다.
	}
}

AActor* ATTPlayer::GetOverlappingActor()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for ( AActor* Actor : OverlappingActors )
	{
		if ( Actor->IsA(AMH_Chair::StaticClass()) )
		{
			return Actor;  // 오버랩된 의자 반환
		}
	}
	return nullptr;
}

void ATTPlayer::OnMyActionPurchase(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed F: Purchase"));
}

void ATTPlayer::OnMyActionInventory(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed TAB: Inventory"));
}

void ATTPlayer::OnMyActionChat(const FInputActionValue& Value)
{
	bIsChatActive = !bIsChatActive;

	if ( bIsChatActive )
	{
		UE_LOG(LogTemp , Warning , TEXT("Pressed Enter: Enable Chat"));
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Pressed Enter: Disable Chat"));
	}
}

void ATTPlayer::InitMainUI()
{
	MainUI = CastChecked<UMainWidget>(CreateWidget(GetWorld() , MainUIFactory));
	if ( MainUI )
	{
		MainUI->AddToViewport();
	}

	TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
	if ( TicketingUI )
	{
		TicketingUI->AddToViewport();
	}
}

void ATTPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// bIsSitting을 개별 클라이언트에 복제
	DOREPLIFETIME(ATTPlayer , bIsSitting);
}

void ATTPlayer::ForceStandUp()
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	if ( Chair && Chair->bIsOccupied && bIsSitting )
	{
		UE_LOG(LogTemp , Warning , TEXT("15초 경과: 강제로 일어납니다."));

		// MainUI 표시
		MainUI->SetVisibleCanvas(true);
		// 좌석 접수 UI 숨기기
		TicketingUI->SetVisibleSwitcher(false);
		ServerSetSitting(false);  // 서버에서 상태 업데이트
		SwitchCamera(bIsThirdPerson);  // 3인칭 시점 복원
	}
}

void ATTPlayer::ServerSetSitting_Implementation(bool _bIsSitting)
{
	bIsSitting = _bIsSitting;  // 서버에서 상태 업데이트

	// 앉기 또는 일어나기 애니메이션 재생
	if ( bIsSitting )
	{
		MulticastSitDown();
	}
	else
	{
		MulticastStandUp();
	}
}

void ATTPlayer::MulticastSitDown_Implementation()
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if ( Chair && Anim )
	{
		Chair->bIsOccupied = true;
		FTransform SittingTransform = Chair->GetSittingTransform();
		SetActorTransform(SittingTransform);
		GetCharacterMovement()->DisableMovement();  // 이동 비활성화
		Anim->PlaySitDownMontage();
	}

	// 의자에 앉은 플레이어에게만 다른 플레이어들을 감추기
	if ( bHideOtherPlayersWhileSitting && IsLocallyControlled() )  // 로컬 플레이어인지 확인
	{
		for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
		{
			ATTPlayer* OtherPlayer = *It;
			if ( OtherPlayer && OtherPlayer != this )  // 자신 이외의 모든 플레이어 감추기
			{
				OtherPlayer->GetMesh()->SetVisibility(false , true);  // 로컬 플레이어 시점에서만 감추기
			}
		}
	}
}

void ATTPlayer::MulticastStandUp_Implementation()
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if ( Chair && Anim )
	{
		Chair->bIsOccupied = false;
		FTransform StandingTransform = Chair->GetStandingTransform();
		SetActorTransform(StandingTransform);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);  // 이동 모드 복원
		Anim->PlayStandUpMontage();
	}

	// 의자에서 일어난 플레이어에게 다시 다른 플레이어들이 보이도록 설정
	if ( bHideOtherPlayersWhileSitting && IsLocallyControlled() )  // 로컬 플레이어인지 확인
	{
		for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
		{
			ATTPlayer* OtherPlayer = *It;
			if ( OtherPlayer && OtherPlayer != this )  // 자신 이외의 모든 플레이어 다시 보이기
			{
				OtherPlayer->GetMesh()->SetVisibility(true , true);  // 로컬 플레이어 시점에서 다시 보이게
			}
		}
	}
}
