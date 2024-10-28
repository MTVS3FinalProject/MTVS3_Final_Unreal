// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNicknameWidget.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API UPlayerNicknameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	FTimerHandle TimerHandle_Retry;
public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , meta = (BindWidget))
	class UTextBlock* Tex_Nickname;

	void UpdateNicknameUI(const FString& _Nickname);
	void ChangeColorNicknameUI(const FColor& RoleColor);
};
