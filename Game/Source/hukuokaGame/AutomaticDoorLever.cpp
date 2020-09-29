//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.cpp
// �T�v			�FItemBase�N���X���p�����鎩���h�A�̃��b�N�A�������Ǘ����郌�o�[
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------


#include "AutomaticDoorLever.h"

AAutomaticDoorLever::AAutomaticDoorLever()
	: m_leverFilter(-1)
	, m_isLeverOn(true)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();
}

void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �v���C���[�Ƀ`�F�b�N����Ă�����
	if (m_isChecked)
	{
		// ���b�V���𔒂�����
	}
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AAutomaticDoorLever::CheckedByPlayer()
{
	// ���o�[�̏�Ԃ𔽓]
	m_isLeverOn = !m_isLeverOn;
}

