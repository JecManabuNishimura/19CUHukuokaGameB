// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Engine.h"						// GEngine���Ăяo���̂ɕK�v�ȃw�b�_
#include "hukuokaGameGameModeBase.h"

void AhukuokaGameGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		// 5�b�ԃ��b�Z�[�W�\��
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("This GameMode is 'hukuokaGameGameModeBase !'"));
	}

}