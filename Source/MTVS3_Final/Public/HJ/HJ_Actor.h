// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HJ_Actor.generated.h"

UCLASS()
class MTVS3_FINAL_API AHJ_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHJ_Actor();

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

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TTSettings|UI")
	// class UWidgetComponent* WidgetComp;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ShowText();

	UFUNCTION()
	void HideText();

	ACharacter* OverlappingPlayer = nullptr;

	class UMainWidget* MainUI;
	void SetMainUI(UMainWidget* InMainUI);
};
