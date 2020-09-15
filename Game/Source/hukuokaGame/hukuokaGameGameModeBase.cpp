// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "hukuokaGameGameModeBase.h"
#include "Engine.h"						// GEngineを呼び出すのに必要なヘッダ

void AhukuokaGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		// 5秒間メッセージ表示
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("This GameMode is 'hukuokaGameGameModeBase !'"));
	}

}