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
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

