//-------------------------------------------------------------------
// �t�@�C��		�FItemCanExamine.cpp
// �T�v			�FItemBase�N���X���p������t�@�C�����̒��ׂ���A�C�e���N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2021/02/10
//-------------------------------------------------------------------

#include "ItemCanExamine.h"

AItemCanExamine::AItemCanExamine()
	: p_photo_mesh_(NULL)
	, distance_from_file_to_player_(150.f)
	, p_playercharacter_(NULL)
	, transform_on_map_(FTransform::Identity)
	, is_show_details_(false)
	, do_file_loc_correction_(false)
{
	p_photo_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhotoMeshComp"));

	if (p_photo_mesh_ != NULL) p_photo_mesh_->SetupAttachment(RootComponent);
}

void AItemCanExamine::BeginPlay()
{
	// �v���C���[���擾
	p_playercharacter_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// �v���C���[���擾�ł��Ȃ���΃��b�Z�[�W�\��
	if (p_playercharacter_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// �}�b�v�ɒu����Ă��鎞�̃g�����X�t�H�[�����i�[
	transform_on_map_ = GetActorTransform();

	Super::BeginPlay();
}

void AItemCanExamine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (do_file_loc_correction_)
	{
		do_file_loc_correction_ = false;

		// �v���C���[�̖ڂ̑O�ɕ\��
		SetActorLocation(p_playercharacter_->GetCameraLocation() + (p_playercharacter_->GetCameraForwardVector() * distance_from_file_to_player_));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), p_playercharacter_->GetCameraLocation()));
	}
}

void AItemCanExamine::CheckedByPlayer()
{
	// ���ݒ��ׂ��Ă��Ȃ��Ȃ�v���C���[�̖ڂ̑O�ɕ\��
	if (!is_show_details_)
	{
		// �擾����炷
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

		// �v���C���[�̑����s�ɂ���
		p_playercharacter_->SetPlayerMoveControlFlag(false);
		p_playercharacter_->SetPlayerCameraControlFlag(false);
		p_playercharacter_->TakeOutTheSmartPhone(false);

		// ���ׂĂ���t���O�𗧂Ă�
		is_show_details_ = true;

		// �ʒu�␳�t���O�𗧂Ă�
		do_file_loc_correction_ = true;
	}
	// ���ݒ��ׂ��Ă���Ȃ猳�̈ʒu�ɖ߂�
	else
	{
		// �v���C���[�̑����L����
		p_playercharacter_->SetPlayerMoveControlFlag(true);
		p_playercharacter_->SetPlayerCameraControlFlag(true);
		p_playercharacter_->TakeOutTheSmartPhone(true);

		// ���ׂĂ���t���O���~�낷
		is_show_details_ = false;

		SetActorTransform(transform_on_map_);
	}
}
