// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawTable.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawTable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALuckyDrawTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp;
	
	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* TextRenderComp;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTextRender(const FText& _RandomSeatNumber);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetColorBlue();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetColorRed();
};
