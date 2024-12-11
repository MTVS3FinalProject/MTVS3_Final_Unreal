// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "GameFramework/Actor.h"
#include "HJ/TTPlayer.h"
#include "LHM/HM_MinimapWidget.h"
#include "MH_MinimapActor.generated.h"

UCLASS()
class MTVS3_FINAL_API AMH_MinimapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMH_MinimapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	class USpringArmComponent* MinimapCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	class USceneCaptureComponent2D* MinimapCapture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	class UPaperSpriteComponent* MinimapSprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TSubclassOf<UTextureRenderTarget2D> MinimapRenderTargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TSubclassOf<UPaperSprite> MinimapSpriteClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	UPaperSpriteComponent* StyleLoungeSprite;

	UPROPERTY()
	ACharacter* Player;
	
	UFUNCTION(BlueprintCallable)
	void ApplyMinimap();
	
	// Render Target 텍스처
	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;

	UFUNCTION()
	void InitializeMinimap(ACharacter* LocalPlayer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UHM_MinimapWidget> MiniMapWidgetClass;
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	class UHM_MinimapWidget* MiniMapUI;

	UFUNCTION()
	void UpdateMinimapWidget();

};
