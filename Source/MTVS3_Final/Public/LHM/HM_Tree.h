// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "HM_Tree.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_Tree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_Tree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Tree;
	UPROPERTY(EditDefaultsOnly)
	TArray <UPhysicsConstraintComponent*> PhysicsConstraints;
	UPROPERTY(EditDefaultsOnly)
	TArray <UStaticMeshComponent*> PhysicsParents;

	UPROPERTY(EditDefaultsOnly)
	TArray<UNiagaraComponent*> NiagaraEffects;
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* NiagaraTemplate;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	TArray<UStaticMeshComponent*> Ticats;
	UPROPERTY(EditDefaultsOnly, Replicated)
	TArray<UStaticMeshComponent*> TicatClips;
	UPROPERTY(ReplicatedUsing=OnRep_TicatVisibility)
	TArray<bool> TicatVisibilities;
	UPROPERTY(ReplicatedUsing=OnRep_TicatVisibility)
	TArray<bool> TicatClipVisibilities;
	UFUNCTION()
	void OnRep_TicatVisibility();
	UFUNCTION()
	void InitializeTicketTabs(int32 TicketTreeId, const FString& TicketImg);
	UFUNCTION()
	void ApplyTicketImage(int32 TicketTreeId, FString TicketImgUrl);
	
};
