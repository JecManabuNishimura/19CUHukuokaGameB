//-------------------------------------------------------------------
// �t�@�C��		�FItemFile.cpp
// �T�v			�FItemFile�N���X���p������y�[�W���߂��邱�Ƃ��o����t�@�C��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2021/02/10
//-------------------------------------------------------------------

#include "ItemFile.h"

AItemFile::AItemFile()
	: p_file_mesh_(NULL)
	, p_datatable_(NULL)
	, p_sound_when_turnpage_(NULL)
	, file_kind_(0)
	, time_open_close_(1.f)
	, time_display_text_(2.f)
	, distance_from_file_to_player_(150.f)
	, p_player_character_(NULL)
	, transform_on_map_(FTransform::Identity)
	, text_linear_color_(FLinearColor::Transparent)
	, left_text_("")
	, right_text_("")
	, page_num_(0)
	, left_page_open_now_num_(-1)
	, count_for_time_open_close_(0.f)
	, is_show_details_(false)
	, can_start_anim_(false)
	, can_turn_page_(false)
	, can_show_detial_(true)
	, do_file_loc_correction_(false)
	, p_widget_comp_(NULL)

{
	// ���I�z��̏�����
	text_in_file_kind_.Reset();

	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// �t�@�C���̃��b�V������
	p_file_mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FileMeshComp"));

	// �E�B�W�F�b�g�R���|�[�l���g����
	p_widget_comp_ = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));

	if (p_file_mesh_ != NULL)	p_file_mesh_->SetupAttachment(RootComponent);
	if (p_widget_comp_ != NULL)	p_widget_comp_->SetupAttachment(p_file_mesh_);
}

void AItemFile::BeginPlay()
{
	// �v���C���[���擾
	p_player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// �v���C���[���擾�ł��Ȃ���΃��b�Z�[�W�\��
	if (p_player_character_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// �f�[�^�e�[�u�����Z�b�g����Ă���΃t�@�C���ɑΉ������e�L�X�g���ڂ�
	if (p_datatable_ != NULL)
	{
		TArray<FFileTextStruct*> datatable_struct_;

		// row�̃f�[�^(struct)*�s��(�e�[�u���S����)�f�[�^���擾
		p_datatable_->GetAllRows<FFileTextStruct>(FString(), datatable_struct_);

		// �v�f��0�̃f�[�^�ɂ͑}���ł��Ȃ��̂�1��ݒ�
		text_in_file_kind_.SetNum(1);

		// �\���̂ɐݒ肳��Ă���t�@�C���ԍ����w�肵�Ă�����e�L�X�g��ݒ�
		if (file_kind_ <= p_datatable_->GetRowNames().Num())
		{
			// ��f�[�^�ł��\�킸�ɑ}�����v�f�� 11 (= 10 + 1)�̃f�[�^�����
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page10, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page9, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page8, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page7, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page6, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page5, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page4, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page3, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page2, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page1, 0);

			// ��f�[�^�̗L���Ńy�[�W����ݒ�
			for (page_num_ = 0; text_in_file_kind_[page_num_] != ""; ++page_num_);

			// �ŏ��ɕs���ȃA�N�Z�X��������邽�߂ɐݒ肵�� 11 �Ԗڂ̃f�[�^���폜
			text_in_file_kind_.SetNum(page_num_);

			// �}�b�v�ɒu����Ă��鎞�̃g�����X�t�H�[�����i�[
			transform_on_map_ = GetActorTransform();

			// �����e�L�X�g�ɍX�V
			UpdatePage(false);
		}
		else this->Destroy();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("DataTable Not be Found !"));
		this->Destroy();
	}

	Super::BeginPlay();
}

void AItemFile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (do_file_loc_correction_)
	{
		do_file_loc_correction_ = false;

		// �v���C���[�̖ڂ̑O�ɕ\��
		SetActorLocation(p_player_character_->GetCameraLocation() + (p_player_character_->GetCameraForwardVector() * distance_from_file_to_player_));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), p_player_character_->GetCameraLocation()));
	}

	count_for_time_open_close_ += DeltaTime;

	// ���ׂ��Ă���Ȃ�J�E���g
	if (is_show_details_)
	{
		if (count_for_time_open_close_ > time_open_close_)
		{
			// �J���A�j���[�V�����̋��t���O�𗧂Ă�
			can_start_anim_ = true;

			// �e�L�X�g�̃t�F�[�h�C������
			text_linear_color_ = FLinearColor(0, 0, 0, (count_for_time_open_close_ - time_display_text_));

			// �e�L�X�g�����S�ɕs�����ɂȂ�����߂��鋖�t���O�𗧂Ă�
			if (count_for_time_open_close_ > (time_display_text_ + 1.0f))
			{
				text_linear_color_ = FLinearColor(0.f, 0.f, 0.f, 1.f);
				can_turn_page_ = true;
			}
		}
	}
	else
	{
		if (!can_show_detial_)
		{
			if (count_for_time_open_close_ > time_open_close_)
			{
				if (abs(GetActorLocation().X - transform_on_map_.GetLocation().X) > 10.f)
				{
					// �v���C���[�̑����L����
					p_player_character_->SetPlayerMoveControlFlag(true);
					p_player_character_->SetPlayerCameraControlFlag(true);
					p_player_character_->TakeOutTheSmartPhone(true);			// �X�}�z���g������ǉ�(�쐬��:�щ_��)

					SetActorTransform(transform_on_map_);

					can_show_detial_ = true;
				}
			}
		}
	}
}

// �v���C���[����̃`�F�b�N�����ꂽ(���ׂ�ꂽ)
void AItemFile::CheckedByPlayer()
{
	if (can_show_detial_)
	{
		// �܂����ׂ��Ă��Ȃ��Ȃ�v���C���[�̖ڂ̑O�ɕ\��
		if (is_show_details_ == false)
		{
			// �擾����炷
			if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

			// �v���C���[�̑����s�ɂ���
			p_player_character_->SetPlayerMoveControlFlag(false);
			p_player_character_->SetPlayerCameraControlFlag(false);
			p_player_character_->TakeOutTheSmartPhone(false);			// �X�}�z���g���Ȃ��ǉ�(�쐬��:�щ_��)

			// ���ׂĂ���t���O�𗧂Ă�
			is_show_details_ = true;

			// �ʒu�␳�t���O�𗧂Ă�
			do_file_loc_correction_ = true;

			count_for_time_open_close_ = 0.0f;

			// �e�L�X�g�X�V
			OnFileUpdatePageEventDispatcher.Broadcast();
		}
		else
		{
			// �y�[�W���߂��鋖���o�Ă���΃e�L�X�g�X�V
			if (can_turn_page_)	UpdatePage(true);

			// �e�L�X�g�X�V
			OnFileUpdatePageEventDispatcher.Broadcast();
		}
	}
}

// �y�[�W���߂���
void AItemFile::UpdatePage(const bool _make_sound)
{
	// ���̍��y�[�W���L���Ȃ�e�L�X�g�؂�ւ�
	if ((left_page_open_now_num_ + 1) < page_num_)
	{
		// �y�[�W���߂��鉹��炷
		if (p_sound_when_turnpage_ != NULL && _make_sound)	UGameplayStatics::PlaySound2D(GetWorld(), p_sound_when_turnpage_);

		++left_page_open_now_num_;
		left_text_ = text_in_file_kind_[left_page_open_now_num_];

		++left_page_open_now_num_;

		// ���̉E�y�[�W���L���Ȃ�e�L�X�g�؂�ւ�
		if ((left_page_open_now_num_) < page_num_)
		{
			right_text_ = text_in_file_kind_[left_page_open_now_num_];
		}
		// �����Ȃ��1������
		else
		{
			right_text_ = " ";
		}
	}
	else
	{
		//------------------------------------------------
		// �{����ă}�b�v�ɖ߂�
		//------------------------------------------------

		// �e�L�X�g�𓧖���
		text_linear_color_ = FLinearColor(0, 0, 0, 0);

		// �J���Ă���y�[�W�������������X�V
		left_page_open_now_num_ = -1;

		// TurnPage();
		if ((left_page_open_now_num_ + 1) < page_num_)
		{
			++left_page_open_now_num_;
			left_text_ = text_in_file_kind_[left_page_open_now_num_];

			++left_page_open_now_num_;

			// ���̉E�y�[�W���L���Ȃ�e�L�X�g�؂�ւ�
			if ((left_page_open_now_num_) < page_num_)
			{
				right_text_ = text_in_file_kind_[left_page_open_now_num_];
			}
			// �����Ȃ��1������
			else
			{
				right_text_ = " ";
			}
		}

		// �t�@�C�����J���Ă���t���O���~�낷
		can_start_anim_ = false;

		is_show_details_ = false;

		can_turn_page_ = false;

		can_show_detial_ = false;

		count_for_time_open_close_ = 0.0f;
	}
}
