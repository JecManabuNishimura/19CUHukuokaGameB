//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.cpp
// �T�v			�FItemBase�N���X���p������E�����Ƃ��o����A�C�e��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/26
//-------------------------------------------------------------------

#include "ItemCanPickup.h"

AItemCanPickup::AItemCanPickup()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItemCanPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AItemCanPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �v���C���[�Ƀ`�F�b�N����Ă�����
	if (m_isChecked)
	{
		// ���b�V���𔒂�����
	}
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AItemCanPickup::CheckedByPlayer()
{
	// ���x���ォ�玩�g������
	this->Destroy();
}


