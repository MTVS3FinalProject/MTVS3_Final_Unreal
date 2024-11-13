// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HallSoundManager.generated.h"

UCLASS()
class MTVS3_FINAL_API AHallSoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHallSoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	class USoundCue* ConcertBGMCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	USoundAttenuation* ConcertAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	FVector ConcertBGMLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	bool bPlayConcertBGM = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	class USoundCue* PlazaBGMCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	USoundAttenuation* PlazaAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	FVector PlazaBGMLocation;
	
	UFUNCTION(BlueprintCallable, Category = "TTSettings|Sound")
	void PlayConcertBGM();

	UFUNCTION(BlueprintCallable, Category = "TTSettings|Sound")
	void PlayPlazaBGM();
};
