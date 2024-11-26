// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawSoundManager.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawSoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALuckyDrawSoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void PlayLuckyDrawBGM();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	class USoundCue* LuckyDrawBGMCue;

	void StopLuckyDrawBGM();
	void EndPlay(EEndPlayReason::Type EndPlayReason);

private:
	UPROPERTY()
	UAudioComponent* BGMAudioComponent;
};
