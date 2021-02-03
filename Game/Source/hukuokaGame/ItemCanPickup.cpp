//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.cpp
// �T�v			�FItemBase�N���X���p������E�����Ƃ��o����A�C�e��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/26
//-------------------------------------------------------------------

#include "ItemCanPickup.h"

AItemCanPickup::AItemCanPickup()
	: player_character_(NULL)
	, cardkey_filter_(0)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItemCanPickup::BeginPlay()
{
	Super::BeginPlay();
	
	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AItemCanPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AItemCanPickup::CheckedByPlayer()
{
	// �Ή�����t���O�𗧂Ă�
	player_character_->SetHaveCardkeyState(cardkey_filter_);

	// �擾����炷
	if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

	// �~�b�V�����ɔ��f����ꍇ�A�X�}�z�̃~�b�V�������A�b�v�f�[�g
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
		} // end if()

	} // end if()

	// ���x���ォ�玩�g������
	this->Destroy();
}