// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "HM_TicketSceneCapture2D.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_TicketSceneCapture2D : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_TicketSceneCapture2D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticket")
	UWidgetComponent* TicketCustomComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticket")
	TSubclassOf<class UHM_TicketCustom> TicketCutomWidget;
	UHM_TicketCustom* TicketCutomUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticket")
	TSubclassOf<class UHM_FinalTicket> FinalTicketWidget;
	class UHM_FinalTicket* FinalTicketUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ticket")
	USceneCaptureComponent2D* SceneCaptureComponent2D;

	void CaptureUsingSceneCapture();

	
private:
	class UTexture2D* ConvertRenderTargetToTexture(UObject* WorldContextObject, UTextureRenderTarget2D* RenderTarget);
};
