// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "hukuokaGameGameModeBase.h"
#include "Engine.h"						// GEngine���Ăяo���̂ɕK�v�ȃw�b�_

void AhukuokaGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		// 5�b�ԃ��b�Z�[�W�\��
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("This GameMode is 'hukuokaGameGameModeBase !'"));
	}

}