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
#include "HJ/TTLuckyDrawGameState.h"
#include "JMH/MH_GameWidget.h"
#include "JMH/PlayerNicknameWidget.h"

class ALuckyDrawManager;
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
	NicknameUIComp->SetRelativeLocationAndRotation(FVector(30.0f , 0 , 0) , FRotator(0 , 90.0f , -90.0f));  // 머리 위로 30 단위 상승
	/*NicknameUIComp->SetupAttachment(RootComponent);
	NicknameUIComp->SetRelativeLocation(FVector(0 , 0 , 100));*/
}

// Called when the game starts or when spawned
void ATTPlayer::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();

	if ( !IsLocallyControlled() )
	{
		if ( GI->GetPlaceState() == EPlaceState::Plaza )
		{
			SetNickname(GetNickname());
		}
		else if ( GI->GetPlaceState() == EPlaceState::LuckyDrawRoom )
		{
			SetRandomSeatNumber(GetRandomSeatNumber());
		}
		SetbIsHost(GetbIsHost());
	}
	else
	{
		SetbIsHost(GI->GetbIsHost());

		if ( NicknameUIFactory )
		{
			NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));

			// C++ 클래스로 캐스팅하여 접근
			NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
			NicknameUI->ChangeColorNicknameUI(FColor::Green);
		}

		auto* PC = Cast<APlayerController>(Controller);
		if ( PC )
		{
			UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			if ( subSys ) subSys->AddMappingContext(IMC_TTPlayer , 0);

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false); // 클릭 시 커서가 사라지지 않도록 설정
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true; // 마우스 커서 표시
		}

		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( !HttpActor2 ) return;

		// TTHallMap에서는 ELuckyDrawState에 따라 추첨 관련 UI 표시할지 결정
		// TTHallMap의 시작은 Plaza(광장)
		if ( GI->GetPlaceState() == EPlaceState::Plaza )
		{
			SwitchCamera(bIsThirdPerson);
			SetNickname(GI->GetNickname());
			// InitMainUI();

			switch ( GI->GetLuckyDrawState() )
			{
			case ELuckyDrawState::Winner:
				// 추첨 당첨 UI 표시
				MainUI->SetWidgetSwitcher(1);
				// HTTP 요청
				HttpActor2->ReqPostGameResult(GI->GetConcertName() , GI->GetLuckyDrawSeatID() , GI->GetAccessToken());
				break;
			case ELuckyDrawState::Loser:
				// 추첨 탈락 UI 표시
				MainUI->SetWidgetSwitcher(2);
				break;
			case ELuckyDrawState::Neutral:
				if ( bShowDebug && GEngine && GetWorld()->GetNetMode() == NM_Client )
				{
					GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green , FString::Printf(TEXT("Show Neutral UI")));
				}
				break;
			}
		}
		else if ( GI->GetPlaceState() == EPlaceState::LuckyDrawRoom )
		{
			SetRandomSeatNumber(GetRandomSeatNumber());
			SwitchCamera(!bIsThirdPerson);
		}

		// 서브레벨 로드/언로드 시 넣을 코드
		//if ( ULevelStreaming* SubLevel = UGameplayStatics::GetStreamingLevel(GetWorld() , TEXT("TTHallMap_Sub")) ) {
		//	if ( SubLevel->IsLevelLoaded() ) {
		//		// 서브레벨이 로드된 상태일 때 처리할 내용
		//	}
		//}
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

	if ( NicknameUIComp && NicknameUIComp->GetVisibleFlag() )
	{
		// P = P0 + vt
		// 카메라 위치
		FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld() , 0)->GetCameraLocation();
		// 체력바와 카메라의 방향 벡터
		FVector NicknameUIDirection = CamLoc - NicknameUIComp->GetComponentLocation();
		//NicknameUIDirection.Z = 0.0f;

		NicknameUIComp->SetWorldRotation(NicknameUIDirection.GetSafeNormal().ToOrientationRotator());
	}

	PrintStateLog();
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
		input->BindAction(IA_Map , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionMap);
		input->BindAction(IA_Cheat1 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat1);
		input->BindAction(IA_Cheat2 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat2);
		input->BindAction(IA_Cheat3 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat3);
		input->BindAction(IA_Cheat4 , ETriggerEvent::Started , this , &ATTPlayer::OnMyActionCheat4);
	}
}

void ATTPlayer::SetNickname(const FString& _Nickname)
{
	Nickname = _Nickname;

	if ( NicknameUIFactory )
	{
		NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));

		// C++ 클래스로 캐스팅하여 접근
		NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
		NicknameUI->UpdateNicknameUI(Nickname);
	}

	if (IsLocallyControlled()) ServerSetNickname(Nickname);
}

void ATTPlayer::ServerSetNickname_Implementation(const FString& _Nickname)
{
	Nickname = _Nickname;
	OnRep_Nickname();
}

void ATTPlayer::OnRep_Nickname()
{
	if ( NicknameUI )
	{
		UE_LOG(LogTemp , Warning , TEXT("OnRep_Nickname: %s") , *Nickname);
		NicknameUI->UpdateNicknameUI(Nickname);
	}
}

void ATTPlayer::SetbIsHost(const bool& _bIsHost)
{
	if (HasAuthority())
	{
		bIsHost = _bIsHost;
	}
}

void ATTPlayer::SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID)
{
	LuckyDrawSeatID = _LuckyDrawSeatID;

	// GI에도 SetLuckyDrawSeatID
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetLuckyDrawSeatID(LuckyDrawSeatID);
}

void ATTPlayer::SetRandomSeatNumber(const int32& _RandomSeatNumber)
{
	RandomSeatNumber = _RandomSeatNumber;

	if ( NicknameUIFactory )
	{
		NicknameUIComp->SetWidgetClass(TSubclassOf<UUserWidget>(NicknameUIFactory));

		// C++ 클래스로 캐스팅하여 접근
		NicknameUI = Cast<UPlayerNicknameWidget>(NicknameUIComp->GetWidget());
		NicknameUI->UpdateNicknameUI(FString::FromInt(RandomSeatNumber));
		UE_LOG(LogTemp , Warning , TEXT("SetRandomSeatNumber: %d") , RandomSeatNumber);
	}

	ServerSetRandomSeatNumber(RandomSeatNumber);
}

void ATTPlayer::ServerSetRandomSeatNumber_Implementation(const int32& _RandomSeatNumber)
{
	RandomSeatNumber = _RandomSeatNumber;
	UE_LOG(LogTemp , Warning , TEXT("ServerSetRandomSeatNumber_Implementation: %d") , RandomSeatNumber);
	OnRep_RandomSeatNumber();
}

void ATTPlayer::OnRep_RandomSeatNumber()
{
	if ( NicknameUI )
	{
		UE_LOG(LogTemp , Warning , TEXT("OnRep_RandomSeatNumber: %d") , RandomSeatNumber);
		NicknameUI->UpdateNicknameUI(FString::FromInt(RandomSeatNumber));
	}
}

void ATTPlayer::ServerLuckyDrawStart_Implementation()
{
	ATTLuckyDrawGameState* GameState = GetWorld()->GetGameState<ATTLuckyDrawGameState>();
	if (GameState)
	{
		GameState->StartLuckyDraw();
	}
}

void ATTPlayer::MulticastLuckyDrawStart_Implementation()
{
	// InitGameUI();
	SwitchCamera(!bIsThirdPerson);
	UTTPlayerAnim* Anim = Cast<UTTPlayerAnim>(GetMesh()->GetAnimInstance());
	if ( Anim )
	{
		Anim->PlaySittingMontage();
	}
	GetCharacterMovement()->DisableMovement();  // 이동 비활성화
}

void ATTPlayer::MulticastMovePlayerToChair_Implementation(const FTransform& TargetTransform)
{
	SetActorTransform(TargetTransform);
}

void ATTPlayer::ClientEndRounds_Implementation()
{
	if (GameUI)
	{
		GameUI->SetWidgetSwitcher(2);  // 우승자 UI 업데이트
	}
}

void ATTPlayer::PrintStateLog()
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ATTPlayerController* PC = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
	ATTPlayerState* PS = Cast<ATTPlayerState>(this->GetPlayerState());
	if ( !GI || !PC || !PS ) return;

	if ( bShowDebug && IsLocallyControlled() )
	{
		// mState를 문자열로 변환
		FString PlaceStateStr = UEnum::GetValueAsString(GI->GetPlaceState());
		FString LuckyDrawStateStr = UEnum::GetValueAsString(GI->GetLuckyDrawState());
		const FString ConStr = GetNetConnection() != nullptr ? TEXT("Client") : TEXT("Server");
		FString BIsHostStr = GetbIsHost() ? TEXT("Manager") : TEXT("Fan");
		FString RandomSeatNumberString = FString::FromInt(RandomSeatNumber);

		FString logStr = FString::Printf(TEXT("%s\n%s\n%s, %s\n%s") , *PlaceStateStr , *LuckyDrawStateStr , *ConStr , *BIsHostStr, *RandomSeatNumberString);
		DrawDebugString(GetWorld() , GetActorLocation() + FVector::UpVector * 100 , logStr , nullptr , FColor::Cyan , 0 , true , 1);
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
		if ( PC && PC->IsLocalController() )
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
		if ( PC && PC->IsLocalController() )
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
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	AHJ_Actor* InteractiveActor = Cast<AHJ_Actor>(GetOverlappingActor());
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( !HttpActor2 || !GI ) return;

	if ( Chair )
	{
		// Chair의 태그를 가져와서 매개변수로 넘김
		FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();
		UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);
		// 의자가 비어 있을 때 상호작용하면 앉는다.
		// 1인칭 시점으로 전환
		if ( !Chair->bIsOccupied )
		{
			UE_LOG(LogTemp , Warning , TEXT("Chair->bIsOccupied = true"));

			// MainUI 숨기기
			MainUI->SetVisibleCanvas(false);
			// 좌석 접수 UI 표시
			TicketingUI->SetVisibleSwitcher(true , 0);
			//TicketingUI->SetWidgetSwitcher(0);
			HttpActor2->ReqPostSeatRegistrationInquiry(GI->GetConcertName() , ChairTag , GI->GetAccessToken());

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
			TicketingUI->SetVisibleSwitcher(false , 0);

			ServerSetSitting(false);
			SwitchCamera(bIsThirdPerson);
		}
		// 의자가 비어 있지 않고 내가 앉아 있지 않으면 아무 일도 안 일어난다.
	}
	else if ( InteractiveActor && InteractiveActor->ActorHasTag(TEXT("SelectConcert")) )
	{
		MainUI->SetWidgetSwitcher(5);
		HttpActor2->ReqGetConcertInfo(GI->GetAccessToken());
	}
	else UE_LOG(LogTemp , Warning , TEXT("Pressed E: fail Interact"));
}

AActor* ATTPlayer::GetOverlappingActor()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for ( AActor* Actor : OverlappingActors )
	{
		if ( Actor->IsA(AMH_Chair::StaticClass()) || Actor->IsA(AHJ_Actor::StaticClass()) )
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
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	AMH_Chair* Chair = Cast<AMH_Chair>(GetOverlappingActor());
	// Chair의 태그를 가져와서 매개변수로 넘김
	FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();
	if ( Chair && HttpActor2 )
	{
		// MainUI 숨기기
		MainUI->SetVisibleCanvas(false);
		// 좌석 경쟁 UI 표시(테스트용)
		TicketingUI->SetVisibleSwitcher(true , 0);
		//TicketingUI->SetWidgetSwitcher(1);
		HttpActor2->ReqPostSeatRegistrationInquiry(GI->GetConcertName() , ChairTag , GI->GetAccessToken());
	}
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

void ATTPlayer::OnMyActionMap(const FInputActionValue& Value)
{
	bIsMapActive = !bIsMapActive;

	if ( bIsMapActive )
	{
		WorldMapUI->SetVisibleSwitcher(true);
		UE_LOG(LogTemp , Warning , TEXT("Pressed M: Enable Map"));
	}
	else
	{
		WorldMapUI->SetVisibleSwitcher(false);
		UE_LOG(LogTemp , Warning , TEXT("Pressed M: Disable Map"));
	}
}

void ATTPlayer::OnMyActionCheat1(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( !GI ) return;
	switch ( GI->GetPlaceState() )
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1 in TTHallMap"));
		bIsCheat1Active = !bIsCheat1Active;
		if ( bIsCheat1Active )
		{
			UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1"));

			// MainUI 숨기기
			MainUI->SetVisibleCanvas(false);
			// 좌석 경쟁 UI 표시
			TicketingUI->SetVisibleSwitcher(true , 1);
			//TicketingUI->SetWidgetSwitcher(1);
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Disable Cheat1"));

			// MainUI 표시
			MainUI->SetVisibleCanvas(true);
			// 좌석 경쟁 UI 숨기기
			TicketingUI->SetVisibleSwitcher(false , 1);
		}
		break;
	case EPlaceState::LuckyDrawRoom:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 1: Enable Cheat1 in TTLuckyDrawMap"));
		GI->SwitchSession(EPlaceState::Plaza);
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SwitchSessionToHall"));
		bIsCheat1Active = !bIsCheat1Active;
		break;
	}
}

void ATTPlayer::OnMyActionCheat2(const FInputActionValue& Value)
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if ( !GI || !Local || !PS ) return;

	switch ( GI->GetPlaceState() )
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		if ( !HttpActor2 ) return;
		UE_LOG(LogTemp , Warning , TEXT("Pressed 2: Enable Cheat2 in TTHallMap"));
		GI->SetLuckyDrawState(ELuckyDrawState::Winner);

		// 추첨 당첨 UI 표시
		MainUI->SetWidgetSwitcher(1);

		// HTTP 통신 요청
		HttpActor2->ReqPostCheatGameResult(GI->GetConcertName() , GI->GetAccessToken());
		break;
	case EPlaceState::LuckyDrawRoom:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 2: Enable Cheat2 in TTLuckyDrawMap"));
		ServerLuckyDrawStart();
		break;
	}
}

void ATTPlayer::OnMyActionCheat3(const FInputActionValue& Value)
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( !GI ) return;

	switch ( GI->GetPlaceState() )
	{
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		UE_LOG(LogTemp , Warning , TEXT("Pressed 3: Enable Cheat3 in TTHallMap"));
		bIsCheat3Active = !bIsCheat3Active;
		GI->SetbIsHost(bIsCheat3Active);
		SetbIsHost(bIsCheat3Active);
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

void ATTPlayer::InitMainUI()
{
	MainUI = Cast<UMainWidget>(CreateWidget(GetWorld() , MainUIFactory));
	if ( MainUI )
	{
		MainUI->AddToViewport();
	}

	TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
	if ( TicketingUI )
	{
		TicketingUI->AddToViewport();
	}

	WorldMapUI = CastChecked<UMH_WorldMap>(CreateWidget(GetWorld() , WorldMapUIFactory));
	if ( WorldMapUI )
	{
		WorldMapUI->AddToViewport();
	}

	ATTPlayerController* MyController = Cast<ATTPlayerController>(GetController());
	if ( MyController )
	{
		MyController->SetMainUI(MainUI);
		MyController->SetTicketingUI(TicketingUI);
		MyController->SetDrawStartTime();
	}

	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if ( HttpActor2 )
	{
		HttpActor2->SetMainUI(MainUI);
		HttpActor2->SetTicketingUI(TicketingUI);
	}
}

void ATTPlayer::InitGameUI()
{
	FTimerHandle SetTextMyNumTimerHandle;
	GetWorldTimerManager().SetTimer(SetTextMyNumTimerHandle, this, &ATTPlayer::SetTextMyNum, 2.0f, false);
}

void ATTPlayer::SetTextMyNum()
{
	GameUI = Cast<UMH_GameWidget>(CreateWidget(GetWorld(), GameUIFactory));
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
	DOREPLIFETIME(ATTPlayer , RandomSeatNumber);
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
		TicketingUI->SetVisibleSwitcher(false , 0);
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
