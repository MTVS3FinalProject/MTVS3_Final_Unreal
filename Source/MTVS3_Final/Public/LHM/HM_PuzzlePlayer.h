// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "HM_PuzzlePlayer.generated.h"

class AHM_PuzzlePiece;

UCLASS()
class MTVS3_FINAL_API AHM_PuzzlePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHM_PuzzlePlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* TPSCameraComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* FPSCameraComp;

	UPROPERTY(EditInstanceOnly , Category = "TTSettings|State")
	bool bIsThirdPerson = true;
	void SwitchCamera(bool _bIsThirdPerson);

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float WalkSpeed = 500.0f;
	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float RunSpeed = 800.0f;

#pragma region 입력
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputMappingContext* IMC_PuzzlePlayer;

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
	class UInputAction* IA_Pickup;
	void OnMyActionPickupPiece(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Launch;
	void OnMyActionLaunchPieceStart(const FInputActionValue& Value);
	void OnMyActionLaunchPieceComplete(const FInputActionValue& Value);
	
	void MyTakePiece();
	void MyReleasePiece();
	void MyLaunchPiece();
	
#pragma endregion	
	
public:
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UHM_AimingWidget> AimingUIFactory;
	UPROPERTY()
	class UHM_AimingWidget* AimingUI;

private:
	// 태어날 때 모든 피스 조각 목록을 기억하고 싶다.
	UPROPERTY()
	TArray<AHM_PuzzlePiece*> PieceList;

public:
	// 잡은 피스 조각의 참조
	UPROPERTY(Replicated)
	class AHM_PuzzlePiece* PickupPieceActor;
	
	// 피스 조각을 잡았을 때 위치
	UPROPERTY(EditDefaultsOnly, Category = Piece)
	class USceneComponent* HandComp;

	// 피스 조각과의 거리 제한
	UPROPERTY(EditDefaultsOnly, Category = Piece)
	float PickupDistance = 300;

	// 피스 조각 잡기와 놓기 기능
	void AttachPiece(AHM_PuzzlePiece* pieceActor);
	void DetachPiece(AHM_PuzzlePiece* pieceActor);
	void LaunchPiece(AHM_PuzzlePiece* pieceActor);

	// 피스 날리기
	//void 

	// --------------- Multiplayer 요소들 ---------------
public:
	UPROPERTY(Replicated, EditDefaultsOnly , BlueprintReadWrite)
	bool bHasPiece = false;

	UPROPERTY(Replicated, EditDefaultsOnly , BlueprintReadWrite)
	bool bIsRightClickPressed = false;
	
	// 피스 잡기 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCTakePiece();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTakePiece(AHM_PuzzlePiece* pieceActor);

	// 피스 놓기 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCReleasePiece();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCReleasePiece(AHM_PuzzlePiece* pieceActor);

	// 피스 날리기
	UFUNCTION(Server, Reliable)
	void ServerRPCLaunchPiece();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCLaunchPiece(AHM_PuzzlePiece* pieceActor);

	// 캐릭터 회전값 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCUpdateRotation(const FRotator& NewRotation);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
