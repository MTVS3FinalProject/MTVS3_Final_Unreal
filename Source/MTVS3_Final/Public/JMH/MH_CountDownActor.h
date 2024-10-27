// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_CountDownActor.generated.h"

UCLASS()
class MTVS3_FINAL_API AMH_CountDownActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMH_CountDownActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* WidgetComp;
	
	//UPROPERTY(VisibleAnywhere, Category = "UI")
    //class UMH_GameStartCountDown* CountDownUI;

	UFUNCTION(NetMulticast, UnReliable)
	void MulticastStartCountDownVisible(bool visible);

};
