// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawChair.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawChair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALuckyDrawChair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp;
	
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraComponent* NiagaraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
	float SeatOffset = 30.0;
	FTransform GetSittingTransform();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ThrowChair();

	UFUNCTION(BlueprintCallable)
	void ResetChair();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPhysicsState(bool bSimulate);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetChair();
	
private:
	FVector OriginalLocation;
	FRotator OriginalRotation;
	bool bIsThrown;

};
