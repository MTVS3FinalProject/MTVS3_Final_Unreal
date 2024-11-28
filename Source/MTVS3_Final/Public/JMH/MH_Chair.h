// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MH_Chair.generated.h"

UCLASS()
class MTVS3_FINAL_API AMH_Chair : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMH_Chair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Boxcomp;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TTSettings|UI")
	// class UWidgetComponent* Widgetcomp;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ShowText();
	
	UFUNCTION()
	void HideText();

#pragma region KHJ
	ACharacter* OverlappingPlayer = nullptr;  // 오버랩된 플레이어 추적용

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
	float SittingLocZ = 32.012604f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
	float StandingLocZ = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
	float SeatOffset = 40.0f;
	FTransform GetSittingTransform();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
	float StandingOffset = 40.0f;
	FTransform GetStandingTransform();

	UPROPERTY(Replicated, VisibleAnywhere, Category = "TTSettings|State")
	bool bIsOccupied;

	UPROPERTY(ReplicatedUsing=OnRep_bIsAvailable, VisibleAnywhere, Category = "TTSettings|State")
	bool bIsAvailable;
	void SetbIsAvailable(bool _bIsAvailable);
	UFUNCTION(Server , Reliable)
	void ServerSetbIsAvailable(bool _bIsAvailable);
	bool GetbIsAvailable() const { return bIsAvailable; };
	UFUNCTION()
	void OnRep_bIsAvailable();
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UPROPERTY()
    class UMainWidget* MainUI;
	void SetMainUI(UMainWidget* InMainUI);

	UPROPERTY()
    class UMH_TicketingWidget* TicketingUI;
	void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RotateChair(bool bIsSitting);

	// bAvailable true면 초록색, false면 빨간색
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeLightColor(bool bAvailable);

	// 의자 상태 업데이트 (GameState 이벤트에 반응)
	UFUNCTION()
	void UpdateChairState();

	// 의자 ID 반환
	int32 GetChairId() const;
#pragma endregion
};
