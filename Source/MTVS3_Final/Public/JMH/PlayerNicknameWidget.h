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

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , meta = (BindWidget))
	class UTextBlock* Tex_Nickname;
};
