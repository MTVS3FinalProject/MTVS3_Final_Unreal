// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

UCLASS()
class MTVS3_FINAL_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputMappingContext* IMC_Camera;

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Move;
	void OnMyActionMove(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputAction* IA_Interact;
	void OnMyActionInteract(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float OrbitRadius = 500.0f;  // 회전 반경

protected:
	UPROPERTY(ReplicatedUsing = OnRep_NetworkedTransform)
	FTransform NetworkedTransform;

	UFUNCTION()
	void OnRep_NetworkedTransform();

	UFUNCTION(Server, Reliable)
	void ServerUpdateTransform(const FTransform& NewTransform);

	void UpdateCameraTransform(const FTransform& NewTransform);

	// 원래의 플레이어 캐릭터 저장
	UPROPERTY(Replicated)
	class ATTPlayer* OriginalPlayer;

	// 원래 플레이어로 돌아가는 함수
	UFUNCTION(Server, Reliable)
	void ServerReturnToOriginalPlayer();

	UPROPERTY(EditAnywhere, Category = "Camera")
	float RotationSpeed = 90.0f; // 초당 90도 회전

public:
	// 초기 설정 함수
	UFUNCTION()
	void SetOriginalPlayer(ATTPlayer* Player);
};
