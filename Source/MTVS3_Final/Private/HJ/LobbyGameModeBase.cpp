// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LobbyGameModeBase.h"

void ALobbyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnableScreenDebug)
	{
#if UE_BUILD_DEVELOPMENT
		GEngine->bEnableOnScreenDebugMessages = false; // 화면 디버그 메시지 비활성화
#endif
	}
}