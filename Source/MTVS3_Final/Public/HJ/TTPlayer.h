// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraPawn.h"
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void UpdateNicknameUI();

	FTimerHandle TimerHandle_Retry;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region 멀티플레이
	UPROPERTY(ReplicatedUsing=OnRep_bIsSitting , BlueprintReadOnly , Category = "TTSettings|State")
	bool bIsSitting;

	UFUNCTION(Server , Reliable)
	void ServerSetSitting(bool _bIsSitting);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastSitDown();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastStandUp();

	UFUNCTION()
	void OnRep_bIsSitting();

	UFUNCTION(Server , Reliable)
	void ServerLuckyDrawStart();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastLuckyDrawStart();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastMovePlayerToChair(const FTransform& TargetTransform);

	UFUNCTION(Client , Reliable)
	void ClientLuckyDrawLose();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayChairGoingUpCameraShake();

	UFUNCTION(Client , Reliable)
	void ClientLuckyDrawWin();

	UFUNCTION(Server , Reliable)
	void ServerLuckyDrawWin();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastLuckyDrawWin();

	UFUNCTION(Client , Reliable)
	void ClientLDWinnerFadeInAnim();
	
	UFUNCTION(Client , Reliable)
	void ClientLDWinnerExitSession();

	// TextRenderComp 사용 X
	// UFUNCTION(NetMulticast , Reliable)
	// void MulticastSetVisibilityTextRender(bool bIsVisible);
	//
	// UFUNCTION(NetMulticast , Reliable)
	// void MulticastSetColorTextRender(const FLinearColor& NewColor);

	UFUNCTION(Server , Reliable)
	void ServerChangeWalkSpeed(bool bIsRunning);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastChangeWalkSpeed(bool bIsRunning);

	UFUNCTION(Client , Reliable)
	void ClientShowLuckyDrawInvitation(bool bIsVisible , const FString& SeatInfo, int32 CompetitionRate);

	UFUNCTION(Server , Reliable , BlueprintCallable)
	void ServerTeleportPlayer(bool bIsToConcertHall);

	UFUNCTION(Client, Reliable)
	void ClientAdjustCamera(FRotator NewRotation);
	
	UFUNCTION(Server , Reliable)
	void ServerSetNewSkeletalMesh(const int32& _AvatarData);

	UFUNCTION(Server , Reliable)
	void ServerNoticeLuckyDrawStart(const FString& _AccessToken, const FString& _SeatId);

	// 클라이언트 모두에게 메시를 변경하는 함수
	UFUNCTION(NetMulticast , Reliable)
	void MulticastSetNewSkeletalMesh(USkeletalMesh* NewMesh);

	UFUNCTION(Server , Reliable)
	void ServerPlayEmojiAnim(const int32& EmojiNum);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastPlayEmojiAnim(const int32& EmojiNum);

	// UFUNCTION()
	// void OnRep_RandomSeatNumber();

	// ====================퍼즐====================
	UPROPERTY(Replicated , VisibleAnywhere , Category = Piece)
	bool bHasPiece = false;
	UPROPERTY(Replicated , VisibleAnywhere , Category = Piece)
	class UStaticMeshComponent* TargetPieceComp;
	UPROPERTY(Replicated , VisibleAnywhere , Category = Piece)
	FTransform TargetPieceTransform;
	UPROPERTY(Replicated , VisibleAnywhere , Category = Piece)
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

	UFUNCTION(Server , Reliable) // 피스 잡기 RPC
	void ServerRPCTakePiece(AHM_PuzzlePiece* pieceActor , UStaticMeshComponent* PieceComp);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCTakePiece(UStaticMeshComponent* PieceComp);

	UFUNCTION(Server , Reliable) // 피스 놓기 RPC
	void ServerRPCReleasePiece();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCReleasePiece(UStaticMeshComponent* PieceComp);

	UFUNCTION(Server , Reliable) // 피스 던지기 RPC
	void ServerRPCLaunchPiece();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCLaunchPiece(UStaticMeshComponent* PieceComp);

	UFUNCTION(Server , Reliable) // 캐릭터 회전값 RPC
	void ServerRPCUpdateRotation(const FRotator& NewRotation);

	UFUNCTION(Server , Reliable) // FPS카메라 회전값 RPC
	void ServerRPCUpdateFPSCameraRotation(const FRotator& FPSCameraNewRotation);

	// 퍼즐 결과 UI
	UFUNCTION(Client, Reliable)
	void Client_UpdatePuzzleUI();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdatePuzzleRankAndVisibility(const TArray<FPlayerScoreInfo>& TopPlayers, int32 TotalPlayers);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateAllPuzzleRanks(const TArray<FPlayerRankInfo>& PlayerRankInfos);
	
#pragma endregion

#pragma region 사운드
	UFUNCTION(BlueprintCallable, Category = "TTSettings|Sound")
	void PlayConcertBGM();
	void PlayConcertBGMAfterDelay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	class USoundBase* LuckyDrawWinnerSound;
#pragma endregion
	
#pragma region 개인 설정
	UPROPERTY(EditAnywhere , Category = "TTSettings|Debug")
	bool bShowDebug = true;
	void PrintStateLog();

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="TTSettings|Custom")
	FColor MyNicknameColor;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float WalkSpeed = 500.0f;
	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float RunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float MaxSittingDuration = 15.0f;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	bool bHideOtherPlayersWhileSitting = false;

	// UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	// bool bMuteSoundWhileSitting = false;
#pragma endregion

#pragma region 플레이어 정보 및 복제 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(ReplicatedUsing=OnRep_Nickname , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString Nickname;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetNickname(const FString& _Nickname);
	UFUNCTION(Server , Reliable)
	void ServerSetNickname(const FString& _Nickname);
	FString GetNickname() const { return Nickname; };
	UFUNCTION()
	void OnRep_Nickname();

	UPROPERTY(ReplicatedUsing=OnRep_TitleNameAndRarity , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString TitleName;
	UPROPERTY(Replicated , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString TitleRarity;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetTitleNameAndRarity(const FString& _TitleName , const FString& _TitleRarity);
	UFUNCTION(Server , Reliable)
	void ServerSetTitleNameAndRarity(const FString& _TitleName , const FString& _TitleRarity);
	FString GetTitleName() const { return TitleName; };
	FString GetTitleRarity() const { return TitleRarity; };
	UFUNCTION()
	void OnRep_TitleNameAndRarity();

	UPROPERTY(ReplicatedUsing=OnRep_bIsHost , BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	bool bIsHost;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetbIsHost(const bool& _bIsHost);
	UFUNCTION(Server , Reliable)
	void ServerSetbIsHost(bool _bIsHost);
	bool GetbIsHost() const { return bIsHost; };
	UFUNCTION()
	void OnRep_bIsHost();
	void UpdateHostVisibility();

	UPROPERTY(Replicated , BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 AvatarData = 1;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetAvatarData(const int32& _AvatarData);
	UFUNCTION(Server , Reliable)
	void ServerSetAvatarData(const int32& _AvatarData);
	int32 GetAvatarData() const { return AvatarData; }

	UPROPERTY(Replicated, VisibleAnywhere, Category = "TTSettings|UserInfo")
	FString AccessToken;
	UFUNCTION(BlueprintCallable, Category = "TTSettings|UserInfo")
	void SetAccessToken(const FString& _AccessToken);
	UFUNCTION(Server, Reliable)
	void ServerSetAccessToken(const FString& _AccessToken);
	FString GetAccessToken() const { return AccessToken; }

	// 랜덤으로 배치된 좌석 번호
	UPROPERTY(ReplicatedUsing=OnRep_RandomSeatNumber , BlueprintReadWrite , VisibleAnywhere ,
		Category = "TTSettings|UserInfo")
	int32 RandomSeatNumber = 0;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetRandomSeatNumber(const int32& _RandomSeatNumber);
	int32 GetRandomSeatNumber() const { return RandomSeatNumber; }
	UFUNCTION()
	void OnRep_RandomSeatNumber();
	UFUNCTION(Server , Reliable)
	void ServerSetRandomSeatNumber(const int32& _RandomSeatNumber);

	// 추첨을 시작할 좌석 ID
	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString LuckyDrawSeatID;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID);
	FString GetLuckyDrawSeatID() const { return LuckyDrawSeatID; };

#pragma endregion
	UPROPERTY(EditDefaultsOnly)
	class UCapsuleComponent* CenterCapsuleComp;
	
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

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "TTSettings|Sequences")
	TObjectPtr<class ULevelSequence> LDWinnerLevelSequence;

	UPROPERTY(Transient)
	class ULevelSequencePlayer* SequencePlayer;

	UFUNCTION(BlueprintCallable , Category = "TTSettings|Avatar")
	void SetNewSkeletalMesh(const int32& _AvatarData);

	// ====================퍼즐====================
	// 잡은 피스 조각의 참조
	UPROPERTY(Replicated)
	class AHM_PuzzlePiece* PickupPieceActor;

	// 피스 조각을 잡았을 때 위치
	UPROPERTY(EditDefaultsOnly , Category = Piece)
	class USceneComponent* HandComp;

	// 피스 조각과의 거리 제한
	UPROPERTY(EditDefaultsOnly , Category = Piece)
	float PickupDistance = 300;

	// 피스 잡은 상태에서 줌인
	float DefaultFOV; // 기본 시야각
	float ZoomedFOV = 60.0f; // 줌인 목표 시야각
	float ZoomDuration = 10.f; // 보간 속도
	FTimerHandle ZoomTimerHandle; // 줌 타이머 핸들

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
	void OnMyActionCheat3(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Cheat4;
	void OnMyActionCheat4(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerResetPuzzlePieces();

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
	UPROPERTY(BlueprintReadWrite, Category = "TTSettings|UI")
	class UMainWidget* MainUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_TicketingWidget> TicketingUIFactory;
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;
	
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_TicketCustom> TicketCustomUIFactory;
	UPROPERTY()
	class UHM_TicketCustom* TicketCustomUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_WorldMap> WorldMapUIFactory;
	UPROPERTY()
	class UMH_WorldMap* WorldMapUI;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "TTSettings|UI")
	class UWidgetComponent* NicknameUIComp;
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UPlayerNicknameWidget> NicknameUIFactory;
	class UPlayerNicknameWidget* NicknameUI;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category = "TTSettings|UI")
	class UWidgetComponent* TitleUIComp;
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UPlayerTitleWidget> TitleUIFactory;
	class UPlayerTitleWidget* TitleUI;

	// TextRenderComp 사용 X
	// UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	// class UTextRenderComponent* TextRenderComp;

	void InitMainUI();

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_GameWidget> GameUIFactory;
	UPROPERTY()
	class UMH_GameWidget* GameUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class ULDTutorialWidget> LDTutorialUIFactory;
	UPROPERTY()
	class ULDTutorialWidget* LDTutorialUI;


	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_AimingWidget> AimingUIFactory;
	UPROPERTY()
	class UHM_AimingWidget* AimingUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_PuzzleWidget> PuzzleUIFactory;
	UPROPERTY()
	class UHM_PuzzleWidget* PuzzleUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_TreeCustomTicketWidget> TreeTicketUIFactory;
	UPROPERTY()
	class UHM_TreeCustomTicketWidget* TreeTicketUI;

	void InitGameUI();
	void SetTextMyNum();

	bool bIsDrawSessionInviteVisible = false;
	void UpdateDrawSessionInviteVisibility(int32 CompetitionRate, const FString& SeatInfo);
#pragma endregion
	
	UPROPERTY(EditAnywhere , Category = "TTSettings|Camera")
	TSubclassOf<class ACameraPawn> CameraPawnFactory;
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "TTSettings|Camera")
	class ACameraPawn* CameraPawn;
	
	bool bIsInCameraMode;

	UFUNCTION(Server, Reliable)
	void ServerNotifyCameraModeChange(bool bNewCameraMode);

	UFUNCTION(Server, Reliable)
	void ServerSpawnCameraPawn();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnCameraModeChanged(bool bNewCameraMode);

	// 카메라 모드에서 플레이어로 돌아오는 함수
	UFUNCTION(Server, Reliable)
	void ServerReturnFromCameraPawn();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnReturnFromCamera();

	// 카메라 폰 참조를 지우는 함수
	void ClearCameraPawnReference();
	
private:
	FTimerHandle StandUpTimerHandle; // 타이머 핸들

	void ForceStandUp();

	//MH
public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TSubclassOf<class AMH_MinimapActor> MinimapActorFac;

	UPROPERTY()
	class AMH_MinimapActor* MinimapActor;

	UFUNCTION()
	void CreateMinimapActor();
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite, Category = "TTSettings|UI")
	class UWidgetComponent*  EmojiComp;
	
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_EmojiImg> EmojiFac;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UMH_EmojiImg* EmojiWidget;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Avatar")
	USkeletalMesh* Avatar1Mesh;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Avatar")
	USkeletalMesh* Avatar2Mesh;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Avatar")
	USkeletalMesh* Avatar3Mesh;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Avatar")
	USkeletalMesh* Avatar4Mesh;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Avatar")
	USkeletalMesh* ManagerMesh;
};
