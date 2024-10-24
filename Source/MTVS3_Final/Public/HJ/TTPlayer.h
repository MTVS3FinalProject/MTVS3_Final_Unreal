// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "TTPlayer.generated.h"

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region 디버그
	UPROPERTY(EditAnywhere , Category = "TTSettings|Debug")
	bool bShowDebug = true;
	//if ( bShowDebug && GEngine && GetWorld()->GetNetMode() == NM_Client )
	//{
	//	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green , FString::Printf(TEXT("Show Winner UI")));
	//}
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

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float WalkSpeed = 500.0f;
	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float RunSpeed = 800.0f;

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

	void InitMainUI();
#pragma endregion

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated , BlueprintReadOnly , Category = "TTSettings|State")
	bool bIsSitting;

	UFUNCTION(Server , Unreliable)
	void ServerSetSitting(bool _bIsSitting);

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastSitDown();

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastStandUp();

private:
	FTimerHandle StandUpTimerHandle;  // 타이머 핸들
	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	float MaxSittingDuration = 15.0f;

	UPROPERTY(EditAnywhere , Category = "TTSettings|Custom")
	bool bHideOtherPlayersWhileSitting = true;

	void ForceStandUp();
};
