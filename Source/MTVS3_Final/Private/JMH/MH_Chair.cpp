// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Chair.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "HJ/TTHallGameState.h"
#include "Net/UnrealNetwork.h"
#include "JMH/MainWidget.h"
#include "HJ/TTPlayer.h"
#include "JMH/MH_TicketingWidget.h"
#include "JMH/MH_Interaction.h"

// Sets default values
AMH_Chair::AMH_Chair()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Boxcomp=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(Boxcomp);

	Boxcomp->OnComponentBeginOverlap.AddDynamic(this , &AMH_Chair::OnBeginOverlap);
	Boxcomp->OnComponentEndOverlap.AddDynamic(this , &AMH_Chair::OnEndOverlap);

	// Widgetcomp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	// Widgetcomp->SetupAttachment(Boxcomp);
}

// Called when the game starts or when spawned
void AMH_Chair::BeginPlay()
{
	Super::BeginPlay();
	// Widgetcomp->SetVisibility(false);

	// GameState의 이벤트에 바인딩
	if (ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>())
	{
		HallState->OnChairStatesUpdated.AddDynamic(this, &AMH_Chair::UpdateChairState);
	}
}

// Called every frame
void AMH_Chair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMH_Chair::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp ,
                               int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if ( TTPlayer && TTPlayer->IsLocallyControlled())
	{
		if (TTPlayer->bHasPiece) return; // 퍼즐 들고 있으면 UI 표시 안 함
		SetMainUI(TTPlayer->MainUI);
		SetTicketingUI(TTPlayer->TicketingUI);

		OverlappingPlayer = TTPlayer;  // 오버랩된 플레이어 추적
		ShowText();
	}
}

void AMH_Chair::OnEndOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                             UPrimitiveComponent* OtherComp , int32 OtherBodyIndex)
{
	ATTPlayer* TTPlayer = Cast<ATTPlayer>(OtherActor);
	if ( TTPlayer && TTPlayer->IsLocallyControlled())
	{
		if (TTPlayer->bHasPiece) return; // 퍼즐 들고 있으면 UI 표시 안 함
		OverlappingPlayer = nullptr;  // 오버랩 해제 시 플레이어 초기화
		HideText();
		// MainUI 표시
		if ( MainUI) MainUI->SetVisibleCanvas(true);
		// 좌석 접수 UI 숨기기
		if ( TicketingUI ) TicketingUI->SetVisibleSwitcher(false, 0);
	}
}

void AMH_Chair::ShowText()
{
	//if (MainUI) MainUI->SetVisibleInteractionCan(true);
	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(MainUI->WBP_InteractionUI);
	if (InteractionUI) InteractionUI->SetActiveWidgetIndex(0);
	// 애니메이션 적용
	InteractionUI->TextOnAnimPlay();

	// WidgetComp(삭제)
	// Widgetcomp->SetVisibility(true);
	//
	// // GetWidget()을 사용하여 위젯 인스턴스를 가져옴
	// UUserWidget* WidgetCompUI = Cast<UUserWidget>(Widgetcomp->GetWidget());
	// if ( WidgetCompUI )
	// {
	// 	// 위젯 인스턴스를 UMH_Interaction으로 캐스팅
	// 	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(WidgetCompUI);
	// 	if ( InteractionUI )
	// 	{
	// 		InteractionUI->SetActiveWidgetIndex(0);
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp , Warning , TEXT("No interaction UI: %s") , *GetName());
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp , Warning , TEXT("No widget found in component: %s") , *GetName());
	// }
}

void AMH_Chair::HideText()
{
	// 애니메이션 적용 안 함
	//MainUI->SetVisibleInteractionCan(false);

	// 애니메이션 적용
	UMH_Interaction* InteractionUI = Cast<UMH_Interaction>(MainUI->WBP_InteractionUI);
	InteractionUI->TextOffAnimPlay();

	// WidgetComp(삭제)
	// Widgetcomp->SetVisibility(false);
}

FTransform AMH_Chair::GetSittingTransform()
{
	// 의자 중심 위치 가져오기
	FVector Location = Boxcomp->GetComponentLocation();
	Location.Z += SittingLocZ;

	// 의자 앞 방향을 기준으로 한 회전값 가져오기
	FRotator Rotation = Boxcomp->GetComponentRotation();

	// 약간의 오프셋을 추가하여 플레이어가 의자와 겹치지 않게
	FVector ForwardOffset = Rotation.Vector() * SeatOffset;  // 의자 앞 방향으로 SeatOffset만큼 이동

	// 최종 위치 계산
	FVector FinalLocation = Location + ForwardOffset;

	// FTransform 생성 및 반환
	return FTransform(Rotation , FinalLocation);
}

FTransform AMH_Chair::GetStandingTransform()
{
	// 의자 중심 위치 가져오기
	FVector Location = Boxcomp->GetComponentLocation();
	Location.Z += StandingLocZ;

	// 의자 앞 방향을 기준으로 한 회전값 가져오기
	FRotator Rotation = Boxcomp->GetComponentRotation();

	// 약간의 오프셋을 추가하여 플레이어가 의자와 겹치지 않게
	FVector ForwardOffset = Rotation.Vector() * StandingOffset;  // 의자 앞 방향으로 StandingOffset만큼 이동

	// 최종 위치 계산
	FVector FinalLocation = Location + ForwardOffset;

	// FTransform 생성 및 반환
	return FTransform(Rotation , FinalLocation);
}

void AMH_Chair::SetbIsAvailable(bool _bIsAvailable)
{
	if (HasAuthority())
	{
		bIsAvailable = _bIsAvailable;
		OnRep_bIsAvailable();
	}
	else
	{
		ServerSetbIsAvailable(_bIsAvailable);
	}
}

void AMH_Chair::ServerSetbIsAvailable_Implementation(bool _bIsAvailable)
{
	bIsAvailable = _bIsAvailable;
	OnRep_bIsAvailable();
}

void AMH_Chair::OnRep_bIsAvailable()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_bIsAvailable called on Server - Chair %s, bIsAvailable: %s"), 
			   *GetName(), bIsAvailable ? TEXT("true") : TEXT("false"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_bIsAvailable called on Client - Chair %s, bIsAvailable: %s"), 
			   *GetName(), bIsAvailable ? TEXT("true") : TEXT("false"));
	}
	
	ChangeLightColor(bIsAvailable);
}

void AMH_Chair::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMH_Chair , bIsOccupied);
	DOREPLIFETIME(AMH_Chair , bIsAvailable);
}

void AMH_Chair::SetMainUI(UMainWidget* InMainUI)
{
	MainUI = InMainUI;  // 전달받은 MainUI 참조 저장
}

void AMH_Chair::SetTicketingUI(UMH_TicketingWidget* InTicketingUI)
{
	TicketingUI = InTicketingUI; // 전달받은 TicketingUI 참조 저장
}

void AMH_Chair::UpdateChairState()
{
	if (ATTHallGameState* HallState = GetWorld()->GetGameState<ATTHallGameState>())
	{
		// 태그에서 의자 ID 추출
		FName FirstTag;
		if (Tags.Num() > 0)
		{
			FirstTag = Tags[0];
			int32 ChairId = FCString::Atoi(*FirstTag.ToString());
            
			bool bShouldBeAvailable = !HallState->GetReservedSeatIdsSet().Contains(ChairId);
            
			if (HasAuthority())
			{
				SetbIsAvailable(bShouldBeAvailable);
			}
		}
	}
}
