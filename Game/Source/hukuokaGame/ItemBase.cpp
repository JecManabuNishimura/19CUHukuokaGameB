//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.cpp
// �T�v			�F�E����A���ׂ���A�쓮�ł���Actor�̊�{�N���X�A
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/11
//-------------------------------------------------------------------


#include "ItemBase.h"

// Sets default values
AItemBase::AItemBase()
	: m_isChecked(false)
	, m_commandName("")
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
