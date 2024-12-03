// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_ErrorMessageManager.generated.h"

UCLASS()
class MTVS3_FINAL_API AMH_ErrorMessageManager : public AActor
{
	GENERATED_BODY()
	//에러 메세지 TMap
	//static TMap<int32, FString> ErrorList;
	
public:	
	// Sets default values for this actor's properties
	AMH_ErrorMessageManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
