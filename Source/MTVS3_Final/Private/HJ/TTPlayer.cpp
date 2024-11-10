// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "JMH/MH_Chair.h"
#include "HJ/TTPlayerAnim.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"   // TActorIterator 정의 포함
#include "Engine/World.h"
#include "JMH/MH_TicketingWidget.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_WorldMap.h"
#include "LHM/TTPlayerController.h"
#include <LHM/HM_HttpActor2.h>
#include "HJ/TTGameInstance.h"
#include <HJ/TTPlayerState.h>
#include <HJ/HJ_Actor.h>
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "HJ/PlayerTitleWidget.h"
#include "HJ/TTLuckyDrawGameState.h"
#include "JMH/MH_GameWidget.h"
#include "JMH/MH_MinimapActor.h"
#include "JMH/PlayerNicknameWidget.h"
#include "LHM/HM_AimingWidget.h"
#include "LHM/HM_PuzzlePiece.h"
#include "LHM/HM_PuzzleWidget.h"

class ALuckyDrawManager;
// Sets default values
ATTPlayer::ATTPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CenterCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CenterCapsule"));
	CenterCapsuleComp->SetupAttachment(RootComponent);
	CenterCapsuleComp->SetCapsuleHalfHeight(88.0f);
	CenterCapsuleComp->SetCapsuleRadius(3.0f);
	
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
	NicknameUIComp->SetupAttachment(GetMesh() , TEXT("head")); // "head" 본에 부착

	TitleUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("TitleUI"));
	// 머리 본(Bone)에 부착
	TitleUIComp->SetupAttachment(GetMesh() , TEXT("head"));

	TextRenderComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("RandomSeatNumberComp"));
	TextRenderComp->SetupAttachment(GetMesh() , TEXT("head"));
	TextRenderComp->SetVisibility(false);

	// 머리 위로 약간 올리기 위한 위치 조정
	NicknameUIComp->SetRelativeLocationAndRotation(FVector(0.46f , -33.0f , -13.0f) , FRotator(75.0f , 270.0f , 180.0f));
	TitleUIComp->SetRelativeLocationAndRotation(FVector(0.46f , -46.0f , -16.0f) , FRotator(75.0f , 270.0f , 180.0f));

	// 퍼즐
	HandComp = CreateDefaultSubobject<USceneComponent>(TEXT("HandComp"));
	HandComp->SetupAttachment(FPSCameraComp);
	HandComp->SetRelativeLocation(FVector(120 , 0 , -40));
}

// Called when the game starts or when spawned
void ATTPlayer::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	if (!IsLocallyControlled())
	{
		if (GI->GetPlaceState() == EPlaceState::Plaza)
		{
			SetNickname(GetNickname());
			OnRep_Nickname();
			SetTitleNameAndRarity(GetTitleName(), GetTitleRarity());
			OnRep_TitleNameAndRarity();
		}
		else if (GI->GetPlaceState() == EPlaceState::LuckyDrawRoom)
		{
			SetRandomSeatNumber(GetRandomSeatNumber());
			OnRep_RandomSeatNumber();
		}
		SetbIsHost(GetbIsHost());
		SetAvatarData(GetAvatarData());
	}
	else // 로컬 플레이어일 때
	{
		if (HasAuthority()) GI->SetbIsHost(true);
		SetbIsHost(GI->GetbIsHost());

		SetAvatarData(GI->GetAvatarData());
		MulticastSetVisibilityTextRender(false);

		if (NicknameUIFactory)
		{
			NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));

			// C++ 클래스로 캐스팅하여 접근
			NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
			NicknameUI->ChangeColorNicknameUI(MyNicknameColor);
		}

		if (TitleUIFactory)
		{
			TitleUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(TitleUIFactory));

			// C++ 클래스로 캐스팅하여 접근
			TitleUI = Cast<UPlayerTitleWidget>(TitleUIComp->GetWidget());
		}

		auto* PC = Cast<APlayerController>(Controller);
		if (PC)
		{
			UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PC->GetLocalPlayer());
			if (subSys) subSys->AddMappingContext(IMC_TTPlayer , 0);

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false); // 클릭 시 커서가 사라지지 않도록 설정
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true; // 마우스 커서 표시
		}

		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if (!HttpActor2) return;

		// TTHallMap에서는 ELuckyDrawState에 따라 추첨 관련 UI 표시할지 결정
		// TTHallMap의 시작은 Plaza(광장)
		if (GI->GetPlaceState() == EPlaceState::Plaza)
		{			
			// SwitchCamera(bIsThirdPerson);
			SetNickname(GI->GetNickname());
			SetTitleNameAndRarity(GI->GetTitleName(), GI->GetTitleRarity());
			// InitMainUI();
			//미니맵 생성
			if (IsLocallyControlled())
			{
				UE_LOG(LogTemp , Warning , TEXT("Locally controlled player: Creating Minimap Actor."));
				CreateMinimapActor();
			}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("Non-locally controlled player or server instance."));
			}

			switch (GI->GetLuckyDrawState())
			{
			case ELuckyDrawState::Winner:
				// 추첨 당첨 UI 표시
				if (MainUI) MainUI->SetWidgetSwitcher(1);
			// HTTP 요청
				HttpActor2->ReqPostGameResult(GI->GetLuckyDrawSeatID() , GI->GetAccessToken());
				break;
			case ELuckyDrawState::Loser:
				// 추첨 탈락 UI 표시
				if (MainUI) MainUI->SetWidgetSwitcher(2);
				break;
			case ELuckyDrawState::Neutral:
				if (bShowDebug && GEngine && GetWorld()->GetNetMode() == NM_Client)
				{
					GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
					                                 FString::Printf(TEXT("Show Neutral UI")));
				}
				break;
			}
		}
		else if (GI->GetPlaceState() == EPlaceState::LuckyDrawRoom)
		{
			// SetRandomSeatNumber(GetRandomSeatNumber());
			SwitchCamera(!bIsThirdPerson);
			SetRandomSeatNumber(0);
		}

		// 서브레벨 로드/언로드 시 넣을 코드
		//if ( ULevelStreaming* SubLevel = UGameplayStatics::GetStreamingLevel(GetWorld() , TEXT("TTHallMap_Sub")) ) {
		//	if ( SubLevel->IsLevelLoaded() ) {
		//		// 서브레벨이 로드된 상태일 때 처리할 내용
		//	}
		//}
	}

	// ====================퍼즐====================
	bReplicates = true;

	if (FPSCameraComp)
	{
		DefaultFOV = FPSCameraComp->FieldOfView;
	}
}

void ATTPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (IsValid(GI) && (EndPlayReason == EEndPlayReason::EndPlayInEditor || EndPlayReason == EEndPlayReason::Quit))
	{
		GI->ClearDestroySessionDelegate();
		GI->ExitSession();
	}
}

// Called every frame
void ATTPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 동기화 지연 있어서 Tick에 추가
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		OnRep_bIsHost();
		OnRep_Nickname();
		OnRep_TitleNameAndRarity();

		if (TitleUIComp && TitleUIComp->GetVisibleFlag())
		{
			// P = P0 + vt
			// 카메라 위치
			FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld() , 0)->GetCameraLocation();
			// 체력바em와 카메라의 방향 벡터
			FVector TitleUIDirection = CamLoc - TitleUIComp->GetComponentLocation();

			TitleUIComp->SetWorldRotation(TitleUIDirection.GetSafeNormal().ToOrientationRotator());
		}
		break;
	case EPlaceState::LuckyDrawRoom:
		OnRep_bIsHost();
		OnRep_RandomSeatNumber();
		break;
	}

	if (NicknameUIComp && NicknameUIComp->GetVisibleFlag())
	{
		// P = P0 + vt
		// 카메라 위치
		FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld() , 0)->GetCameraLocation();
		// 체력바em와 카메라의 방향 벡터
		FVector NicknameUIDirection = CamLoc - NicknameUIComp->GetComponentLocation();
		//NicknameUIDirection.Z = 0.0f;

		NicknameUIComp->SetWorldRotation(NicknameUIDirection.GetSafeNormal().ToOrientationRotator());
	}

	PrintStateLog();

	// ====================퍼즐====================
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && PC->IsLocalController())
	{
		if (!bHasPiece)
		{
			FTransform ttt = FTransform(GetControlRotation());
			Direction = ttt.TransformVector(Direction);

			AddMovementInput(Direction , 1);
			Direction = FVector::ZeroVector;
		}
		else if (bHasPiece)
		{
			if (PC && PC->IsLocalController())
			{
				FRotator ControlRotation = PC->GetControlRotation();

				if (!bIsThirdPerson) // 1인칭
				{
					// Set the actor's rotation to match the camera's rotation
					FRotator NewRotation = FRotator(0.0f , ControlRotation.Yaw , 0.0f);
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
}

// Called to bind functionality to input
void ATTPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
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
		input->BindAction(IA_Map , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionMap);
		input->BindAction(IA_Cheat1 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat1);
		input->BindAction(IA_Cheat2 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat2);
		input->BindAction(IA_Cheat3 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat3);
		input->BindAction(IA_Cheat4 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat4);
		input->BindAction(IA_Piece , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionPickupPiece);
		input->BindAction(IA_Zoom , ETriggerEvent::Triggered , this , &ATTPlayer::OnMyActionZoomInPiece);
		input->BindAction(IA_Zoom , ETriggerEvent::Completed , this , &ATTPlayer::OnMyActionZoomOutPiece);
	}
}

void ATTPlayer::SetNickname(const FString& _Nickname)
{
	ServerSetNickname(_Nickname);
}

void ATTPlayer::ServerSetNickname_Implementation(const FString& _Nickname)
{
	Nickname = _Nickname;
	OnRep_Nickname();
}

void ATTPlayer::OnRep_Nickname()
{
	// 위젯이 없다면 생성
	if (!NicknameUI && NicknameUIFactory)
	{
		NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));
		NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
	}

	// 닉네임 업데이트
	if (NicknameUI)
	{
		NicknameUI->UpdateNicknameUI(Nickname);
	}
}

void ATTPlayer::SetTitleNameAndRarity(const FString& _TitleName, const FString& _TitleRarity)
{
	ServerSetTitleNameAndRarity(_TitleName, _TitleRarity);
}

void ATTPlayer::ServerSetTitleNameAndRarity_Implementation(const FString& _TitleName, const FString& _TitleRarity)
{
	TitleName = _TitleName;
	TitleRarity = _TitleRarity;
	OnRep_TitleNameAndRarity();
}

void ATTPlayer::OnRep_TitleNameAndRarity()
{
	// 위젯이 없다면 생성
	if (!TitleUI && TitleUIFactory)
	{
		TitleUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(TitleUIFactory));
		TitleUI = Cast<UPlayerTitleWidget>(TitleUIComp->GetWidget());
	}

	// 닉네임 업데이트
	if (TitleUI)
	{
		TitleUI->UpdateTitleNameUI(TitleName);
		TitleUI->ChangeColorTitleNameUI(TitleRarity);
	}
}

void ATTPlayer::SetbIsHost(const bool& _bIsHost)
{
	ServerSetbIsHost(_bIsHost);
}

void ATTPlayer::ServerSetbIsHost_Implementation(bool _bIsHost)
{
	bIsHost = _bIsHost;
	MulticastSetbIsHost(bIsHost);

	if (bIsHost == true)
	{
		ServerSetNewSkeletalMesh(0);
	}
	else
	{
		ServerSetNewSkeletalMesh(GetAvatarData());
	}
}

void ATTPlayer::MulticastSetbIsHost_Implementation(bool _bIsHost)
{
	if (bIsHost == true)
	{
		GetMesh()->SetOnlyOwnerSee(true);
		GetCapsuleComponent()->SetOnlyOwnerSee(true);
		CenterCapsuleComp->SetOnlyOwnerSee(true);
		NicknameUIComp->SetOnlyOwnerSee(true);
		TitleUIComp->SetOnlyOwnerSee(true);
	}
	else
	{
		GetMesh()->SetOnlyOwnerSee(false);
		GetCapsuleComponent()->SetOnlyOwnerSee(false);
		CenterCapsuleComp->SetOnlyOwnerSee(false);
		NicknameUIComp->SetOnlyOwnerSee(false);
		TitleUIComp->SetOnlyOwnerSee(false);
	}
}

void ATTPlayer::OnRep_bIsHost()
{
	if (GetbIsHost() == true)
	{
		SetNewSkeletalMesh(0);
		MulticastSetbIsHost(true);
	}
	else
	{
		SetNewSkeletalMesh(GetAvatarData());
		MulticastSetbIsHost(false);
	}
}

void ATTPlayer::SetAvatarData(const int32& _AvatarData)
{
	ServerSetAvatarData(_AvatarData);
}

void ATTPlayer::ServerSetAvatarData_Implementation(const int32& _AvatarData)
{
	AvatarData = _AvatarData;
}

void ATTPlayer::SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID)
{
	LuckyDrawSeatID = _LuckyDrawSeatID;

	// GI에도 SetLuckyDrawSeatID
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI) GI->SetLuckyDrawSeatID(LuckyDrawSeatID);
}

void ATTPlayer::SetRandomSeatNumber(const int32& _RandomSeatNumber)
{
	if (HasAuthority())
	{
		RandomSeatNumber = _RandomSeatNumber;
		OnRep_RandomSeatNumber();
	}
	else
	{
		ServerSetRandomSeatNumber(_RandomSeatNumber);
	}
}

void ATTPlayer::ServerSetRandomSeatNumber_Implementation(const int32& _RandomSeatNumber)
{
	RandomSeatNumber = _RandomSeatNumber;
}

void ATTPlayer::OnRep_RandomSeatNumber()
{
	if (!NicknameUI && NicknameUIFactory)
	{
		NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));
		NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
	}

	// 닉네임UI, TextRenderComp 업데이트
	if (NicknameUI)
	{
		NicknameUI->UpdateNicknameUI(FString::FromInt(GetRandomSeatNumber()));
		TextRenderComp->SetText(FText::FromString(FString::FromInt(GetRandomSeatNumber())));
	}
}

void ATTPlayer::ServerSetNewSkeletalMesh_Implementation(const int32& _AvatarData)
{
	USkeletalMesh* Avatar1Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar2Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar3Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar4Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* ManagerMesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/KHJ/Assets/SM_Manager.SM_Manager'"));

	switch (_AvatarData)
	{
	case 1:
		if (Avatar1Mesh) MulticastSetNewSkeletalMesh(Avatar1Mesh);
		break;
	case 2:
		if (Avatar2Mesh) MulticastSetNewSkeletalMesh(Avatar2Mesh);
		break;
	case 3:
		if (Avatar3Mesh) MulticastSetNewSkeletalMesh(Avatar3Mesh);
		break;
	case 4:
		if (Avatar4Mesh) MulticastSetNewSkeletalMesh(Avatar4Mesh);
		break;
	default:
		if (ManagerMesh) MulticastSetNewSkeletalMesh(ManagerMesh);
		break;
	}
}

void ATTPlayer::MulticastSetNewSkeletalMesh_Implementation(USkeletalMesh* NewMesh)
{
	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

void ATTPlayer::ServerTeleportPlayer_Implementation(bool bIsToConcertHall)
{
	FVector TargetLocation = bIsToConcertHall ? FVector(19 , -4962 , 516) : FVector(20680 , 6260 , 3092);
	FRotator TargetRotation = bIsToConcertHall ? FRotator(0 , 90 , 0) : FRotator(0 , 170 , 0);

	TeleportTo(TargetLocation , TargetRotation);
}

void ATTPlayer::ServerLuckyDrawStart_Implementation()
{
	ATTLuckyDrawGameState* GameState = GetWorld()->GetGameState<ATTLuckyDrawGameState>();
	if (GameState && GameState->bIsStartRound != true)
	{
		GameState->StartLuckyDraw();
	}
}

void ATTPlayer::MulticastLuckyDrawStart_Implementation()
{
	// InitGameUI();
	SwitchCamera(!bIsThirdPerson);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Anim)
	{
		Anim->PlaySittingMontage();
	}
	GetCharacterMovement()->DisableMovement(); // 이동 비활성화
}

void ATTPlayer::MulticastMovePlayerToChair_Implementation(const FTransform& TargetTransform)
{
	SetActorTransform(TargetTransform);
}

void ATTPlayer::ClientLuckyDrawLose_Implementation()
{
	PlayChairGoingUpCameraShake();
	if (GameUI)
	{
		GameUI->HideWidget();
	}
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI)
	{
		GI->SetLuckyDrawState(ELuckyDrawState::Loser);
		GI->SwitchSession(EPlaceState::Plaza);
	}
}

void ATTPlayer::ClientLuckyDrawWin_Implementation()
{
	if (GameUI)
	{
		GameUI->SetWidgetSwitcher(2); // 우승자 UI 업데이트
	}

	// 서버 RPC, 멀티캐스트 RPC 필요
	ServerLuckyDrawWin();

	if (LDWinnerLevelSequence)
	{
		// 레벨 시퀀스의 월드 위치 설정
		FMovieSceneSequencePlaybackSettings Settings;
		Settings.bAutoPlay = true;
		Settings.LoopCount.Value = -1; // -1은 무한 반복, 특정 횟수 반복은 원하는 정수로 설정

		// 레벨 시퀀스 플레이어 생성
		ALevelSequenceActor* SequenceActor;
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
			GetWorld() ,
			LDWinnerLevelSequence ,
			Settings ,
			SequenceActor
		);

		if (SequencePlayer)
		{
			// 시퀀스 재생 시작
			SequencePlayer->Play();
		}

		FTimerHandle LDWinnerTimerHandle;
		GetWorldTimerManager().SetTimer(LDWinnerTimerHandle , this , &ATTPlayer::ClientLDWinnerExitSession , 6.0f ,
		                                false);
	}
}

void ATTPlayer::ServerLuckyDrawWin_Implementation()
{
	MulticastLuckyDrawWin();
}

void ATTPlayer::MulticastLuckyDrawWin_Implementation()
{
	SetActorLocationAndRotation(FVector(0.0f , 2510.0f , 490.0f) , FRotator(0.0f , -90.0f , 0.0f));
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Anim) Anim->PlayDancingMontage();
}

void ATTPlayer::ClientLDWinnerExitSession_Implementation()
{
	if (SequencePlayer)
	{
		SequencePlayer->Stop();
	}

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI)
	{
		GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
		GI->SwitchSession(EPlaceState::Plaza);
	}
}

void ATTPlayer::MulticastSetVisibilityTextRender_Implementation(bool bIsVisible)
{
	TextRenderComp->SetVisibility(bIsVisible);
}

void ATTPlayer::MulticastSetColorTextRender_Implementation(FColor NewColor)
{
	TextRenderComp->SetTextRenderColor(NewColor);
}

void ATTPlayer::ServerChangeWalkSpeed_Implementation(bool bIsRunning)
{
	MulticastChangeWalkSpeed(bIsRunning);
}

void ATTPlayer::MulticastChangeWalkSpeed_Implementation(bool bIsRunning)
{
	if (bIsRunning == true)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ATTPlayer::ClientShowLuckyDrawInvitation_Implementation(bool bIsVisible , int32 CompetitionRate)
{
	ATTPlayerController* TTPC = Cast<ATTPlayerController>(GetController());
	if (TTPC)
	{
		TTPC->SetDrawStartTime();
	}
	
	bIsDrawSessionInviteVisible = bIsVisible; // 현재 추첨 세션 초대 UI 가시성 상태를 저장
	UpdateDrawSessionInviteVisibility(CompetitionRate);
}

void ATTPlayer::UpdateDrawSessionInviteVisibility(int32 CompetitionRate)
{
	if (!bHasPiece && bIsDrawSessionInviteVisible)
	{
		// 큐브를 들고 있지 않고 초대 UI가 보이는 상태
		// 티켓팅 UI 표시, 메인 UI 숨기기
		if (MainUI) MainUI->SetVisibleCanvas(false);
		if (TicketingUI)
		{
			TicketingUI->SetVisibleSwitcher(true , 1);
			TicketingUI->SetTextCompetitionRate(CompetitionRate);
		}
	}
	else
	{
		// 큐브를 들고 있거나 초대 UI가 안 보이는 상태
		// 메인 UI 표시, 티켓팅 UI 숨기기
		if (MainUI) MainUI->SetVisibleCanvas(true);
		if (TicketingUI)
		{
			TicketingUI->SetVisibleSwitcher(false , 1);
			TicketingUI->SetTextCompetitionRate(CompetitionRate);
		}
	}
}

void ATTPlayer::ServerNoticeLucyDrawStart_Implementation()
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if (HttpActor2)
	{
		HttpActor2->ReqPostNoticeGameStart(TEXT("2024A113") ,
		                                   GI->GetAccessToken());
	}
}

void ATTPlayer::MyTakePiece()
{
	ZoomOut();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector2D MousePosition;
	if (PC->GetMousePosition(MousePosition.X , MousePosition.Y))
	{
		// 마우스 위치를 월드 위치(Start)와 방향(Direction)으로 변환
		FVector Start , Dir;
		PC->DeprojectScreenPositionToWorld(MousePosition.X , MousePosition.Y , Start , Dir);

		float TraceDistance = 1000.0f;
		FVector End = Start + (Dir * TraceDistance);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("MouseTrace")) , true , this);
		TraceParams.bReturnPhysicalMaterial = false;
		TraceParams.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult , Start , End , ECC_Visibility , TraceParams);

		DrawDebugLine(GetWorld() , Start , End , FColor::Blue , false , 5.f , 0 , 1.f);

		if (bHit && HitResult.GetComponent())
		{
			AHM_PuzzlePiece* HitPiece = Cast<AHM_PuzzlePiece>(HitResult.GetActor());
			if (HitPiece)
			{
				UStaticMeshComponent* HitComponent = Cast<UStaticMeshComponent>(HitResult.GetComponent());
				if (HitComponent && HitPiece->GetAllPieces().Contains(HitComponent))
				{
					// 소유되지 않은 상태이며, 특정 태그가 있는지 확인
					if (!HitPiece->IsComponentOwned(HitComponent))
					{
						for (int32 i = 1; i <= 9; ++i)
						{
							FString TagName = FString::Printf(TEXT("Piece%d") , i);
							if (HitComponent->ComponentHasTag(FName(*TagName)))
							{
								GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Orange ,
								                                 FString::Printf(
									                                 TEXT("Found piece with tag: %s") , *TagName));
								TargetPieceComp = HitComponent;

								// 서버로 소유 요청을 보냄
								ServerRPCTakePiece(HitPiece , TargetPieceComp);
								break;
							}
						}
					}
					if (PuzzleUI) PuzzleUI->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

void ATTPlayer::MyReleasePiece()
{
	if (false == bHasPiece || false == IsLocallyControlled())
		return;
	else
	{
		ServerRPCReleasePiece();
	}
}

void ATTPlayer::MyLaunchPiece()
{
	if (false == bHasPiece || false == IsLocallyControlled())
		return;
	else
	{
		ServerRPCLaunchPiece();
	}
}

void ATTPlayer::ZoomIn()
{
	if (FPSCameraComp)
	{
		// 기존 타이머가 있다면 종료
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);

		bIsZoomingIn = true;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle , [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView , ZoomedFOV , GetWorld()->GetDeltaSeconds() ,
			                                ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView , ZoomedFOV , 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				//bIsZoomingIn = true; // 줌 인 상태로 고정
			}
		} , 0.01f , true);
	}
}

void ATTPlayer::ZoomOut()
{
	if (FPSCameraComp)
	{
		// 기존 타이머가 있다면 종료
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);

		bIsZoomingIn = false;
		GetWorld()->GetTimerManager().SetTimer(ZoomTimerHandle , [this]()
		{
			float NewFOV = FMath::FInterpTo(FPSCameraComp->FieldOfView , DefaultFOV , GetWorld()->GetDeltaSeconds() ,
			                                ZoomDuration);
			FPSCameraComp->SetFieldOfView(NewFOV);

			if (FMath::IsNearlyEqual(FPSCameraComp->FieldOfView , DefaultFOV , 0.1f))
			{
				GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
				//bIsZoomingIn = false; // 줌 아웃 상태로 고정
			}
		} , 0.01f , true);
	}
}

void ATTPlayer::AttachPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCameraOnPiece(false);

	// 클라이언트와 서버 모두에서 실행될 회전 로직
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		FRotator ControlRotation = PC->GetControlRotation();
		// 캐릭터의 회전을 카메라 회전과 동기화
		SetActorRotation(FRotator(0.0f , ControlRotation.Yaw , 0.0f));
		PC->SetViewTargetWithBlend(this);
	}

	if (PieceComp)
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

void ATTPlayer::DetachPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCameraOnPiece(true);

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

void ATTPlayer::LaunchPiece(UStaticMeshComponent* PieceComp)
{
	if (!PieceComp) return;
	SwitchCameraOnPiece(true);

	PieceComp->SetSimulatePhysics(true);
	PieceComp->SetEnableGravity(true);
	PieceComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PieceComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	PieceComp->SetCollisionProfileName(TEXT("PuzzlePiece"));
	PieceComp->SetNotifyRigidBodyCollision(true);
	PieceComp->SetGenerateOverlapEvents(true);

	FVector LaunchDirection = FPSCameraComp->GetForwardVector();
	float LaunchSpeed = 6000.0f;
	FVector LaunchVelocity = LaunchDirection * LaunchSpeed;
	PieceComp->AddImpulse(LaunchVelocity , NAME_None , true);

	if (HasAuthority())
	{
		TargetPieceTransform = PieceComp->GetComponentTransform();
	}
	TargetPieceComp = nullptr;
}

void ATTPlayer::ServerRPCTakePiece_Implementation(AHM_PuzzlePiece* pieceActor , UStaticMeshComponent* PieceComp)
{
	if (!HasAuthority() || !pieceActor || !PieceComp || pieceActor->IsComponentOwned(PieceComp)) return;

	pieceActor->SetComponentOwner(PieceComp , this);
	PickupPieceActor = pieceActor;
	TargetPieceComp = PieceComp;
	bHasPiece = true;

	// 모든 클라이언트에 알림
	MulticastRPCTakePiece(PieceComp);
}

void ATTPlayer::MulticastRPCTakePiece_Implementation(UStaticMeshComponent* PieceComp)
{
	AttachPiece(PieceComp);
}

void ATTPlayer::ServerRPCReleasePiece_Implementation()
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

void ATTPlayer::MulticastRPCReleasePiece_Implementation(UStaticMeshComponent* PieceComp)
{
	if (PieceComp)
	{
		DetachPiece(PieceComp); // 피스 분리
		TargetPieceComp = nullptr;
	}
}

void ATTPlayer::ServerRPCLaunchPiece_Implementation()
{
	if (bHasPiece && TargetPieceComp && PickupPieceActor)
	{
		// 피스의 마지막 소유자를 현재 소유자로 설정
		PickupPieceActor->LastOwners.Add(TargetPieceComp , this);

		bHasPiece = false;
		PickupPieceActor->ClearComponentOwner(TargetPieceComp);
		MulticastRPCLaunchPiece(TargetPieceComp);
		PickupPieceActor = nullptr;
		TargetPieceComp = nullptr;
	}
	bIsZoomingIn = false;
}

void ATTPlayer::MulticastRPCLaunchPiece_Implementation(UStaticMeshComponent* PieceComp)
{
	if (PieceComp)
	{
		LaunchPiece(PieceComp);
		TargetPieceComp = nullptr;
	}
}

void ATTPlayer::ServerRPCUpdateRotation_Implementation(const FRotator& NewRotation)
{
	// 서버에서 캐릭터 회전 적용
	SetActorRotation(NewRotation);
}

void ATTPlayer::ServerRPCUpdateFPSCameraRotation_Implementation(const FRotator& FPSCameraNewRotation)
{
	if (FPSCameraComp)
	{
		FPSCameraComp->SetWorldRotation(FPSCameraNewRotation);
	}
}

void ATTPlayer::PrintStateLog()
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ATTPlayerController* PC = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
	ATTPlayerState* PS = Cast<ATTPlayerState>(this->GetPlayerState());
	if (!GI || !PC || !PS) return;

	if (bShowDebug && IsLocallyControlled())
	{
		// mState를 문자열로 변환
		FString PlaceStateStr = UEnum::GetValueAsString(GI->GetPlaceState());
		FString LuckyDrawStateStr = UEnum::GetValueAsString(GI->GetLuckyDrawState());
		const FString ConStr = GetNetConnection() != nullptr ? TEXT("Client") : TEXT("Server");
		FString BIsHostStr = GetbIsHost() ? TEXT("Manager") : TEXT("Fan");
		FString RandomSeatNumberString = FString::FromInt(RandomSeatNumber);

		FString logStr = FString::Printf(TEXT("%s\n%s\n%s, %s\n%s") , *PlaceStateStr , *LuckyDrawStateStr , *ConStr ,
		                                 *BIsHostStr , *RandomSeatNumberString);
		DrawDebugString(GetWorld() , GetActorLocation() + FVector::UpVector * 100 , logStr , nullptr , FColor::Cyan ,
		                0 , true , 1);
	}
}

void ATTPlayer::SwitchCamera(bool _bIsThirdPerson)
{
	if (_bIsThirdPerson) // 3인칭 모드
	{
		GetMesh()->SetOwnerNoSee(false);
		FPSCameraComp->SetActive(false);
		TPSCameraComp->SetActive(true);
		NicknameUIComp->SetOwnerNoSee(false);

		APlayerController* PC = Cast<APlayerController>(GetController());
		// 플레이어의 회전 방향과 카메라 정렬
		if (PC && PC->IsLocalController())
		{
			PC->SetViewTargetWithBlend(this); // 부드러운 시점 전환
		}
	}
	else // 1인칭 모드
	{
		FPSCameraComp->SetActive(true);
		TPSCameraComp->SetActive(false);
		NicknameUIComp->SetOwnerNoSee(true);
		GetMesh()->SetOwnerNoSee(true);

		// 플레이어의 회전 방향과 카메라 정렬
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC && PC->IsLocalController())
		{
			// 캐릭터의 메시를 1인칭 시점에서 보이지 않게 설정
			GetMesh()->SetOwnerNoSee(true);

			// 캐릭터의 현재 회전 방향으로 카메라를 맞춤
			FRotator ControlRotation = GetActorRotation();
			PC->SetControlRotation(ControlRotation);

			PC->SetViewTargetWithBlend(this); // 부드러운 시점 전환
		}
	}
}

void ATTPlayer::SwitchCameraOnPiece(bool _bIsThirdPerson)
{
	bIsThirdPerson = _bIsThirdPerson;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->IsLocalController()) return;

	if (bIsThirdPerson) // 3인칭 모드
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());

		GetMesh()->SetOwnerNoSee(false);
		FPSCameraComp->SetActive(false);
		TPSCameraComp->SetActive(true);
		NicknameUIComp->SetOwnerNoSee(false);

		PC->SetViewTargetWithBlend(this); // 부드러운 시점 전환
	}
	else // 1인칭 모드
	{
		FPSCameraComp->SetActive(true);
		TPSCameraComp->SetActive(false);
		NicknameUIComp->SetOwnerNoSee(true);
		GetMesh()->SetOwnerNoSee(true);

		if (!bHasPiece)
		{
			if (PC && PC->IsLocalController())
			{
				// 캐릭터의 메시를 1인칭 시점에서 보이지 않게 설정
				GetMesh()->SetOwnerNoSee(true);

				// 캐릭터의 현재 회전 방향으로 카메라를 맞춤
				FRotator ControlRotation = GetActorRotation();
				PC->SetControlRotation(ControlRotation);

				PC->SetViewTargetWithBlend(this); // 부드러운 시점 전환
			}
		}
		else if (bHasPiece)
		{
			// 마우스 커서 숨기기 및 게임 모드 설정
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());

			FRotator NewRotation = FRotator(0.0f , GetControlRotation().Yaw , 0.0f);
			SetActorRotation(FRotator(NewRotation));
			PC->SetViewTargetWithBlend(this);

			// 서버에 회전 값 전달
			ServerRPCUpdateRotation(NewRotation);
			ServerRPCUpdateFPSCameraRotation(PC->GetControlRotation());

			// 마우스 감도 설정
			if (APlayerCameraManager* CameraMgr = PC->PlayerCameraManager)
			{
				CameraMgr->ViewPitchMin = -20.0f;
				CameraMgr->ViewPitchMax = 50.0f;
			}
		}
	}
}

void ATTPlayer::SetNewSkeletalMesh(const int32& _AvatarData)
{
	USkeletalMesh* Avatar1Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar2Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar3Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* Avatar4Mesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT(
			"/Script/Engine.SkeletalMesh'/Game/JMH/Mesh/Player01/ShovedReactionWithSpin_UE.ShovedReactionWithSpin_UE'"));
	USkeletalMesh* ManagerMesh = LoadObject<USkeletalMesh>(
		nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/KHJ/Assets/SM_Manager.SM_Manager'"));

	switch (_AvatarData)
	{
	case 1:
		if (Avatar1Mesh) GetMesh()->SetSkeletalMesh(Avatar1Mesh);
		break;
	case 2:
		if (Avatar2Mesh) GetMesh()->SetSkeletalMesh(Avatar2Mesh);
		break;
	case 3:
		if (Avatar3Mesh) GetMesh()->SetSkeletalMesh(Avatar3Mesh);
		break;
	case 4:
		if (Avatar4Mesh) GetMesh()->SetSkeletalMesh(Avatar4Mesh);
		break;
	default:
		if (ManagerMesh) GetMesh()->SetSkeletalMesh(ManagerMesh);
		break;
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
	if (bIsEnableLook)
	{
		AddControllerPitchInput(-v.Y);
		AddControllerYawInput(v.X);
	}

	if (!bIsThirdPerson && bHasPiece)
	{
		AddControllerPitchInput(-v.Y);
		AddControllerYawInput(v.X);

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC && PC->IsLocalController())
		{
			// 마우스 커서 숨기기 및 게임 모드 설정
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());

			FRotator ControlRotation = PC->GetControlRotation();
			FRotator NewRotation = FRotator(0.0f , ControlRotation.Yaw , 0.0f);
			SetActorRotation(NewRotation);
			PC->SetViewTargetWithBlend(this);

			// 서버에 회전 값 전달
			ServerRPCUpdateRotation(NewRotation);
			ServerRPCUpdateFPSCameraRotation(ControlRotation);
		}
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
	ServerChangeWalkSpeed(true);
}

void ATTPlayer::OnMyActionRunComplete(const FInputActionValue& Value)
{
	ServerChangeWalkSpeed(false);
}

void ATTPlayer::OnMyActionInteract(const FInputActionValue& Value)
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	AHJ_Actor* InteractiveActor = Cast<AHJ_Actor>(GetOverlappingActor());
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!HttpActor2 || !GI) return;

	if (Chair)
	{
		// Chair의 태그를 가져와서 매개변수로 넘김
		FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();
		UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);
		// 의자가 비어 있을 때 상호작용하면 앉는다.
		// 1인칭 시점으로 전환
		if (!Chair->bIsOccupied)
		{
			UE_LOG(LogTemp , Warning , TEXT("Chair->bIsOccupied = true"));

			// MainUI 숨기기
			MainUI->SetVisibleCanvas(false);
			// 좌석 접수 UI 표시
			TicketingUI->SetVisibleSwitcher(true , 0);
			//TicketingUI->SetWidgetSwitcher(0);
			HttpActor2->ReqGetSeatRegistrationInquiry(ChairTag , GI->GetAccessToken());

			ServerSetSitting(true);

			// 의자의 회전값 가져오기
			FRotator ChairRotation = Chair->GetActorRotation();

			// 카메라의 회전을 의자에 맞게 설정
			FPSCameraComp->SetWorldRotation(ChairRotation); // 의자 방향으로 카메라 회전 설정
			SwitchCamera(!bIsThirdPerson);

			// 15초 후에 자동으로 일어나도록 타이머 시작
			GetWorld()->GetTimerManager().SetTimer(
				StandUpTimerHandle , this , &ATTPlayer::ForceStandUp , MaxSittingDuration , false);
		}
		// 의자가 비어 있지 않고 내가 앉아 있으면 일어난다.
		// 3인칭 시점으로 전환
		else if (Chair->bIsOccupied && bIsSitting)
		{
			UE_LOG(LogTemp , Warning , TEXT("Chair->bIsOccupied = false"));

			// MainUI 표시
			MainUI->SetVisibleCanvas(true);
			// 좌석 접수 UI 숨기기
			TicketingUI->SetVisibleSwitcher(false , 0);

			ServerSetSitting(false);
			SwitchCamera(bIsThirdPerson);
		}
		// 의자가 비어 있지 않고 내가 앉아 있지 않으면 아무 일도 안 일어난다.
	}
	else if (InteractiveActor && InteractiveActor->ActorHasTag(TEXT("SelectConcert")))
	{
		MainUI->SetWidgetSwitcher(5);
		HttpActor2->ReqGetConcertInfo(GI->GetAccessToken() , this);
	}
	else if (InteractiveActor && InteractiveActor->ActorHasTag(TEXT("Customizing")))
	{
		// 티켓 커스터마이징 액터 상호작용 시 UI 표시
	}
	else UE_LOG(LogTemp , Warning , TEXT("Pressed E: fail Interact"));
}

AActor* ATTPlayer::GetOverlappingActor()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->IsA(AMH_Chair::StaticClass()) || Actor->IsA(AHJ_Actor::StaticClass()))
		{
			return Actor;
		}
	}
	return nullptr;
}

void ATTPlayer::OnMyActionPurchase(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed F: Purchase"));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	if (Chair && HttpActor2)
	{
		// Chair의 태그를 가져와서 매개변수로 넘김
		FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();
		// MainUI 숨기기
		MainUI->SetVisibleCanvas(false);
		// 좌석 경쟁 UI 표시(테스트용)
		TicketingUI->SetVisibleSwitcher(true , 0);
		//TicketingUI->SetWidgetSwitcher(1);
		HttpActor2->ReqGetSeatRegistrationInquiry(ChairTag , GI->GetAccessToken());
	}
}

void ATTPlayer::OnMyActionInventory(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed TAB: Inventory"));
}

void ATTPlayer::OnMyActionChat(const FInputActionValue& Value)
{
	bIsChatActive = !bIsChatActive;

	if (bIsChatActive)
	{
		UE_LOG(LogTemp , Warning , TEXT("Pressed Enter: Enable Chat"));
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Pressed Enter: Disable Chat"));
	}
	if (MainUI) MainUI->ShowChatUI();
}

void ATTPlayer::OnMyActionMap(const FInputActionValue& Value)
{
	bIsMapActive = !bIsMapActive;
	if (!IsLocallyControlled()) return;

	if (bIsMapActive)
	{
		WorldMapUI->SetVisibleSwitcher(true);
		GetCharacterMovement()->DisableMovement();
		UE_LOG(LogTemp , Warning , TEXT("Pressed M: Enable Map"));
	}
	else
	{
		WorldMapUI->SetVisibleSwitcher(false);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); // 이동 모드 복원
		UE_LOG(LogTemp , Warning , TEXT("Pressed M: Disable Map"));
	}
}

void ATTPlayer::OnMyActionCheat1(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;
	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1 in TTHallMap"));
		if (GetbIsHost() || HasAuthority())
		{
			bIsCheat1Active = !bIsCheat1Active;
			if (bIsCheat1Active)
			{
				UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1"));
				ServerNoticeLucyDrawStart();

				// MainUI 숨기기
				MainUI->SetVisibleCanvas(false);
				// 좌석 경쟁 UI 표시
				TicketingUI->SetVisibleSwitcher(true , 1);
			}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Disable Cheat1"));

				// MainUI 표시
				MainUI->SetVisibleCanvas(true);
				// 좌석 경쟁 UI 숨기기
				TicketingUI->SetVisibleSwitcher(false , 1);
			}
		}
		break;
	case EPlaceState::LuckyDrawRoom:
		if (GetbIsHost())
		{
			if (GI)
			{
				GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
				GI->SwitchSession(EPlaceState::Plaza);
				UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1 in TTLuckyDrawMap(Switch Session)"));
			}
		}
		else UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1 in TTLuckyDrawMap(Nothing)"));
	/*GI->SwitchSession(EPlaceState::Plaza);
	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SwitchSessionToHall"));
	bIsCheat1Active = !bIsCheat1Active;*/
		break;
	}
}

void ATTPlayer::OnMyActionCheat2(const FInputActionValue& Value)
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if (!GI || !Local || !PS) return;

	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		if (!HttpActor2) return;
		UE_LOG(LogTemp , Warning , TEXT("Pressed 2: Enable Cheat2 in TTHallMap"));
		if (GetbIsHost() || HasAuthority())
		{
			GI->SetLuckyDrawState(ELuckyDrawState::Winner);

			// 추첨 당첨 UI 표시
			MainUI->SetWidgetSwitcher(1);

			// HTTP 통신 요청
			HttpActor2->ReqPostCheatGameResult(GI->GetAccessToken());
		}
		break;
	case EPlaceState::LuckyDrawRoom:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 2: Enable Cheat2 in TTLuckyDrawMap"));
		if (GetbIsHost() || HasAuthority()) ServerLuckyDrawStart();
		break;
	}
}

void ATTPlayer::OnMyActionCheat3(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;

	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 3: Enable Cheat3 in TTHallMap"));
		bIsHost = !bIsHost;
		if (GI) GI->SetbIsHost(bIsHost);
		SetbIsHost(bIsHost);
		break;
	case EPlaceState::LuckyDrawRoom:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 3: Enable Cheat3 in TTLuckyDrawMap"));
		break;
	}
}

void ATTPlayer::OnMyActionCheat4(const FInputActionValue& Value)
{
	UE_LOG(LogTemp , Warning , TEXT("Pressed 4: Cheat4"));
	bShowDebug = !bShowDebug;
}

void ATTPlayer::OnMyActionPickupPiece(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI->GetPlaceState() == EPlaceState::LuckyDrawRoom) return;
	
	if (bIsZoomingIn && bHasPiece)
	{
		MyLaunchPiece();
		if (AimingUI) AimingUI->SetVisibility(ESlateVisibility::Hidden);
		if (PuzzleUI) PuzzleUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (bHasPiece)
	{
		MyReleasePiece();
		if (PuzzleUI) PuzzleUI->SetVisibility(ESlateVisibility::Hidden);
	}
	//else if(!bIsZoomingIn && !bHasPiece)
	else if (!bHasPiece)
	{
		MyTakePiece();
	}
}

void ATTPlayer::OnMyActionZoomInPiece(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI->GetPlaceState() == EPlaceState::LuckyDrawRoom) return;
	
	if (bHasPiece && !bIsZoomingIn && !bIsThirdPerson)
	{
		bIsZoomingIn = true;
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Visible);
		}
		ZoomIn();
	}
}

void ATTPlayer::OnMyActionZoomOutPiece(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI->GetPlaceState() == EPlaceState::LuckyDrawRoom) return;
	
	if (bHasPiece && bIsZoomingIn && !bIsThirdPerson)
	{
		bIsZoomingIn = false;
		if (AimingUI)
		{
			AimingUI->SetVisibility(ESlateVisibility::Hidden);
		}
		ZoomOut();
	}
}

void ATTPlayer::InitMainUI()
{
	MainUI = Cast<UMainWidget>(CreateWidget(GetWorld() , MainUIFactory));
	if (MainUI)
	{
		MainUI->AddToViewport();
	}

	TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
	if (TicketingUI)
	{
		TicketingUI->AddToViewport();
	}

	WorldMapUI = CastChecked<UMH_WorldMap>(CreateWidget(GetWorld() , WorldMapUIFactory));
	if (WorldMapUI)
	{
		WorldMapUI->AddToViewport();
	}

	AimingUI = Cast<UHM_AimingWidget>(CreateWidget(GetWorld() , AimingUIFactory));
	if (AimingUI)
	{
		AimingUI->AddToViewport();
		AimingUI->SetVisibility(ESlateVisibility::Hidden);
	}

	PuzzleUI = Cast<UHM_PuzzleWidget>(CreateWidget(GetWorld() , PuzzleUIFactory));
	if (PuzzleUI)
	{
		PuzzleUI->AddToViewport();
		PuzzleUI->SetVisibility(ESlateVisibility::Hidden);
	}

	ATTPlayerController* TTPC = Cast<ATTPlayerController>(GetController());
	if (TTPC)
	{
		TTPC->SetMainUI(MainUI);
		TTPC->SetTicketingUI(TicketingUI);
	}

	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if (HttpActor2)
	{
		HttpActor2->SetMainUI(MainUI);
		HttpActor2->SetTicketingUI(TicketingUI);
	}
}

void ATTPlayer::InitGameUI()
{
	SetTextMyNum();

	/*FTimerHandle SetTextMyNumTimerHandle;
	GetWorldTimerManager().SetTimer(SetTextMyNumTimerHandle , this , &ATTPlayer::SetTextMyNum , 4.0f , false);*/
}

void ATTPlayer::SetTextMyNum()
{
	GameUI = Cast<UMH_GameWidget>(CreateWidget(GetWorld() , GameUIFactory));
	if (GameUI)
	{
		GameUI->AddToViewport(1);
		GameUI->SetWidgetSwitcher(1);
		GameUI->SetOnlyVisibleMyNum(false);
		GameUI->SetTextMyNum(GetRandomSeatNumber());
	}
}

void ATTPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// bIsSitting을 개별 클라이언트에 복제
	DOREPLIFETIME(ATTPlayer , bIsSitting);
	DOREPLIFETIME(ATTPlayer , Nickname);
	DOREPLIFETIME(ATTPlayer , TitleName);
	DOREPLIFETIME(ATTPlayer , TitleRarity);
	DOREPLIFETIME(ATTPlayer , RandomSeatNumber);
	DOREPLIFETIME(ATTPlayer , AvatarData);
	DOREPLIFETIME(ATTPlayer , bIsHost);

	// 퍼즐
	DOREPLIFETIME(ATTPlayer , bHasPiece);
	DOREPLIFETIME(ATTPlayer , bIsZoomingIn);
	DOREPLIFETIME(ATTPlayer , PickupPieceActor);
	DOREPLIFETIME(ATTPlayer , TargetPieceComp);
	DOREPLIFETIME(ATTPlayer , TargetPieceTransform);
}

void ATTPlayer::ForceStandUp()
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	if (Chair && Chair->bIsOccupied && bIsSitting)
	{
		UE_LOG(LogTemp , Warning , TEXT("15초 경과: 강제로 일어납니다."));

		// MainUI 표시
		MainUI->SetVisibleCanvas(true);
		// 좌석 접수 UI 숨기기
		TicketingUI->SetVisibleSwitcher(false , 0);
		ServerSetSitting(false); // 서버에서 상태 업데이트
		SwitchCamera(bIsThirdPerson); // 3인칭 시점 복원
	}
}

void ATTPlayer::ServerSetSitting_Implementation(bool _bIsSitting)
{
	bIsSitting = _bIsSitting; // 서버에서 상태 업데이트

	// 앉기 또는 일어나기 애니메이션 재생
	if (bIsSitting)
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
	if (Chair && Anim)
	{
		Chair->bIsOccupied = true;
		FTransform SittingTransform = Chair->GetSittingTransform();
		this->SetActorTransform(SittingTransform);
		GetCharacterMovement()->DisableMovement(); // 이동 비활성화
		Anim->PlaySitDownMontage();
	}

	// 의자에 앉은 플레이어에게만 다른 플레이어들을 감추기
	if (bHideOtherPlayersWhileSitting && IsLocallyControlled()) // 로컬 플레이어인지 확인
	{
		for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
		{
			ATTPlayer* OtherPlayer = *It;
			if (OtherPlayer && OtherPlayer != this) // 자신 이외의 모든 플레이어 감추기
			{
				OtherPlayer->GetMesh()->SetVisibility(false , true); // 로컬 플레이어 시점에서만 감추기
			}
		}
	}
}

void ATTPlayer::MulticastStandUp_Implementation()
{
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if (Chair && Anim)
	{
		Chair->bIsOccupied = false;
		FTransform StandingTransform = Chair->GetStandingTransform();
		SetActorTransform(StandingTransform);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); // 이동 모드 복원
		Anim->PlayStandUpMontage();
	}

	// 의자에서 일어난 플레이어에게 다시 다른 플레이어들이 보이도록 설정
	if (bHideOtherPlayersWhileSitting && IsLocallyControlled()) // 로컬 플레이어인지 확인
	{
		for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
		{
			ATTPlayer* OtherPlayer = *It;
			if (OtherPlayer && OtherPlayer != this) // 자신 이외의 모든 플레이어 다시 보이기
			{
				OtherPlayer->GetMesh()->SetVisibility(true , true); // 로컬 플레이어 시점에서 다시 보이게
			}
		}
	}
}

//MH
void ATTPlayer::CreateMinimapActor()
{
	MinimapActor = GetWorld()->SpawnActor<AMH_MinimapActor>(MinimapActorFac);
	if (MinimapActor)
	{
		MinimapActor->InitializeMinimap(this);
	}
}
