// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "TTPlayer.generated.h"

class AHM_PuzzlePiece;

UCLASS()
class MTVS3_FINAL_API ATTPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATTPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	void UpdateNicknameUI();

	FTimerHandle TimerHandle_Retry;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region 멀티플레이
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated , BlueprintReadOnly , Category = "TTSettings|State")
	bool bIsSitting;

	UFUNCTION(Server , Unreliable)
	void ServerSetSitting(bool _bIsSitting);

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastSitDown();

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastStandUp();

	UFUNCTION(Server , Reliable)
	void ServerSetNickname(const FString& _Nickname);

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastSetNickname();

	UFUNCTION(Server , Reliable)
	void ServerSetRandomSeatNumber(const int32& _RandomSeatNumber);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetRandomSeatNumber();

	UFUNCTION(Server, Reliable)
	void ServerLuckyDrawStart();

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastLuckyDrawStart();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMovePlayerToChair(const FTransform& TargetTransform);

	UFUNCTION(Client, Reliable)
	void ClientLuckyDrawLose();
	
	UFUNCTION(Client, Reliable)
	void ClientLuckyDrawWin();

	UFUNCTION(Client, Reliable)
	void ClientLDWinnerExitSession();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetVisibilityTextRender(bool bIsVisible);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetColorTextRender(FColor NewColor);

	UFUNCTION(Server , Unreliable)
	void ServerChangeWalkSpeed(bool bIsRunning);

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastChangeWalkSpeed(bool bIsRunning);

	UFUNCTION(Client, Reliable)
	void ClientShowLuckyDrawInvitation(bool bIsVisible, int32 CompetitionRate);

	UFUNCTION()
	void OnRep_Nickname();
	
	UFUNCTION()
	void OnRep_RandomSeatNumber();

	UFUNCTION(Server , Reliable, BlueprintCallable)
	void ServerTeleportPlayer(bool bIsToConcertHall);

	// UFUNCTION()
	// void OnRep_RandomSeatNumber();

	// ====================퍼즐====================
	UPROPERTY(Replicated, VisibleAnywhere , Category = Piece)
	bool bHasPiece = false;
	UPROPERTY(Replicated, VisibleAnywhere , Category = Piece)
	class UStaticMeshComponent* TargetPieceComp;
	UPROPERTY(Replicated, VisibleAnywhere , Category = Piece)
	FTransform TargetPieceTransform;
	UPROPERTY(Replicated, VisibleAnywhere , Category = Piece)
	bool bIsZoomingIn = false;
	
	// 피스 조각 잡기와 놓기 던지기 기능
	void MyTakePiece();
	void MyReleasePiece();
	void MyLaunchPiece();

	void ZoomIn();
	void ZoomOut();
	
	void AttachPiece(UStaticMeshComponent* PieceComp);
	void DetachPiece(UStaticMeshComponent* PieceComp);
	void LaunchPiece(UStaticMeshComponent* PieceComp);
	
	UFUNCTION(Server, Reliable)	// 피스 잡기 RPC
	void ServerRPCTakePiece(AHM_PuzzlePiece* pieceActor, UStaticMeshComponent* PieceComp);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTakePiece(UStaticMeshComponent* PieceComp);
	
	UFUNCTION(Server, Reliable) // 피스 놓기 RPC
	void ServerRPCReleasePiece();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCReleasePiece(UStaticMeshComponent* PieceComp);
	
	UFUNCTION(Server, Reliable) // 피스 던지기 RPC
	void ServerRPCLaunchPiece();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCLaunchPiece(UStaticMeshComponent* PieceComp);
	
	UFUNCTION(Server, Reliable) // 캐릭터 회전값 RPC
	void ServerRPCUpdateRotation(const FRotator& NewRotation);
	
	UFUNCTION(Server, Reliable) // FPS카메라 회전값 RPC
	void ServerRPCUpdateFPSCameraRotation(const FRotator& FPSCameraNewRotation);

#pragma endregion

#pragma region 개인 설정
	UPROPERTY(EditAnywhere , Category = "TTSettings|Debug")
	bool bShowDebug = true;
	void PrintStateLog();

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float WalkSpeed = 500.0f;
	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float RunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float MaxSittingDuration = 15.0f;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	bool bHideOtherPlayersWhileSitting = false;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	bool bMuteSoundWhileSitting = false;
#pragma endregion

#pragma region 플레이어 정보
	UPROPERTY(/*Replicated*/ReplicatedUsing=OnRep_Nickname , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString Nickname;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetNickname(const FString& _Nickname);
	FString GetNickname() const { return Nickname; };

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	bool bIsHost;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetbIsHost(const bool& _bIsHost);
	bool GetbIsHost() const { return bIsHost; };

	// 추첨을 시작할 좌석 ID
	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString LuckyDrawSeatID;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID);
	FString GetLuckyDrawSeatID() const { return LuckyDrawSeatID; };

	// 랜덤으로 배치된 좌석 번호
	UPROPERTY(/*Replicated*/ReplicatedUsing=OnRep_RandomSeatNumber , BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 RandomSeatNumber = -1;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetRandomSeatNumber(const int32& _RandomSeatNumber);
	int32 GetRandomSeatNumber() const { return RandomSeatNumber; }
#pragma endregion

	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* TPSCameraComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* FPSCameraComp;

	UPROPERTY(EditInstanceOnly , Category = "TTSettings|State")
	bool bIsThirdPerson = true;
	void SwitchCamera(bool _bIsThirdPerson);
	void SwitchCameraOnPiece(bool _bIsThirdPerson);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sequences")
	TObjectPtr<class ULevelSequence> LDWinnerLevelSequence;

	UPROPERTY(Transient)
	class ULevelSequencePlayer* SequencePlayer;

	// ====================퍼즐====================
	// 잡은 피스 조각의 참조
	UPROPERTY(Replicated)
	class AHM_PuzzlePiece* PickupPieceActor;
	
	// 피스 조각을 잡았을 때 위치
	UPROPERTY(EditDefaultsOnly, Category = Piece)
	class USceneComponent* HandComp;

	// 피스 조각과의 거리 제한
	UPROPERTY(EditDefaultsOnly, Category = Piece)
	float PickupDistance = 300;

	// 피스 잡은 상태에서 줌인
	float DefaultFOV; // 기본 시야각
	float ZoomedFOV = 60.0f;  // 줌인 목표 시야각
	float ZoomDuration = 10.f; // 보간 속도
	FTimerHandle ZoomTimerHandle;   // 줌 타이머 핸들
	
#pragma region 입력
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputMappingContext* IMC_TTPlayer;

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Move;
	void OnMyActionMove(const FInputActionValue& Value);

	FVector Direction;

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_EnableLook;
	void OnMyActionEnableLookStart(const FInputActionValue& Value);
	void OnMyActionEnableLookComplete(const FInputActionValue& Value);
	bool bIsEnableLook = false;

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Look;
	void OnMyActionLook(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Jump;
	void OnMyActionJumpStart(const FInputActionValue& Value);
	void OnMyActionJumpComplete(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Run;
	void OnMyActionRunStart(const FInputActionValue& Value);
	void OnMyActionRunComplete(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Interact;
	void OnMyActionInteract(const FInputActionValue& Value);

	AActor* GetOverlappingActor();

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Purchase;
	void OnMyActionPurchase(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Inventory;
	void OnMyActionInventory(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Chat;
	UPROPERTY(VisibleAnywhere , Category = "TTSettings|Settings")
	bool bIsChatActive;
	void OnMyActionChat(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Map;
	UPROPERTY(VisibleAnywhere , Category = "TTSettings|State")
	bool bIsMapActive;
	void OnMyActionMap(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Cheat1;
	UPROPERTY(VisibleAnywhere , Category = "TTSettings|State")
	bool bIsCheat1Active;
	void OnMyActionCheat1(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Cheat2;
	void OnMyActionCheat2(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Cheat3;
	UPROPERTY(VisibleAnywhere , Category = "TTSettings|State")
	bool bIsCheat3Active;
	void OnMyActionCheat3(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Cheat4;
	void OnMyActionCheat4(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Piece;
	void OnMyActionPickupPiece(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Zoom;
	void OnMyActionZoomInPiece(const FInputActionValue& Value);
	void OnMyActionZoomOutPiece(const FInputActionValue& Value);
#pragma endregion

#pragma region UI
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMainWidget> MainUIFactory;
	UPROPERTY()
	class UMainWidget* MainUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_TicketingWidget> TicketingUIFactory;
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_WorldMap> WorldMapUIFactory;
	UPROPERTY()
	class UMH_WorldMap* WorldMapUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	class UWidgetComponent* NicknameUIComp;
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UPlayerNicknameWidget> NicknameUIFactory;
	class UPlayerNicknameWidget* NicknameUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	class UTextRenderComponent* TextRenderComp;
	
	void InitMainUI();

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_GameWidget> GameUIFactory;
	UPROPERTY()
	class UMH_GameWidget* GameUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_AimingWidget> AimingUIFactory;
	UPROPERTY()
	class UHM_AimingWidget* AimingUI;
	
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_PuzzleWidget> PuzzleUIFactory;
	UPROPERTY()
	class UHM_PuzzleWidget* PuzzleUI;
	
	void InitGameUI();
	void SetTextMyNum();

	bool bIsDrawSessionInviteVisible = false;
	void UpdateDrawSessionInviteVisibility(int32 CompetitionRate);
#pragma endregion

private:
	FTimerHandle StandUpTimerHandle;  // 타이머 핸들
	
	void ForceStandUp();
};
