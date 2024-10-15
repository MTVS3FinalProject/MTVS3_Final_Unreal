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

	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* TPSCameraComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* FPSCameraComp;

	UPROPERTY(EditAnywhere, Category = "Default|TTSettings")
	bool bIsThirdPerson = true;
	void SwitchCamera(bool _bIsThirdPerson);

	UPROPERTY(EditAnywhere , Category = "Default|TTSettings")
	float WalkSpeed = 500.0f;
	UPROPERTY(EditAnywhere , Category = "Default|TTSettings")
	float RunSpeed = 800.0f;

#pragma region 입력
	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputMappingContext* IMC_TTPlayer;

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Move;
	void OnMyActionMove(const FInputActionValue& Value);

	FVector Direction;

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_EnableLook;
	void OnMyActionEnableLookStart(const FInputActionValue& Value);
	void OnMyActionEnableLookComplete(const FInputActionValue& Value);
	bool bIsEnableLook = false;

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Look;
	void OnMyActionLook(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Jump;
	void OnMyActionJumpStart(const FInputActionValue& Value);
	void OnMyActionJumpComplete(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Run;
	void OnMyActionRunStart(const FInputActionValue& Value);
	void OnMyActionRunComplete(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Interact;
	void OnMyActionInteract(const FInputActionValue& Value);

	AActor* GetOverlappingActor();

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Purchase;
	void OnMyActionPurchase(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Inventory;
	void OnMyActionInventory(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "Default|Input")
	class UInputAction* IA_Chat;
	UPROPERTY(EditAnywhere , Category = "Default|Settings")
	bool bIsChatActive;
	void OnMyActionChat(const FInputActionValue& Value);
#pragma endregion

	UPROPERTY(EditAnywhere , Category = "Default|UI")
	TSubclassOf<class UUserWidget> MainUIFactory;
	UPROPERTY()
	class UUserWidget* MainUI;

	void InitMainUI();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	bool bIsSitting;

	UFUNCTION(Server , Unreliable)
	void ServerSetSitting(bool _bIsSitting);

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastSitDown();

	UFUNCTION(NetMulticast , Unreliable)
	void MulticastStandUp();
};
