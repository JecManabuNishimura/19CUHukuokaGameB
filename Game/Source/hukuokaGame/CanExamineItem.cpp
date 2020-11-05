//-------------------------------------------------------------------
// �t�@�C��		�FCanExamineItem.cpp
// �T�v			�FItemBase�N���X���p������t�@�C�����̒��ׂ���A�C�e���N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/26
//-------------------------------------------------------------------

#include "CanExamineItem.h"

ACanExamineItem::ACanExamineItem()
	: player_character_(NULL)
	, file_mesh_(NULL)
	, datatable_(NULL)
	, transform_on_map_(FTransform::Identity)
	, text_linear_color_(FLinearColor::Transparent)
	, file_kind_(0)
	, page_num_(1)
	, left_page_open_now_num_(-1)
	, time_open_close_(1.f)
	, time_display_text_(2.f)
	, distance_from_file_to_player_(150.f)
	, count_for_time_open_close_(0.f)
	, is_show_details_(false)
	, can_start_anim_(false)
	, can_turn_page_(false)
	, can_show_detial_(true)
	, widget_comp_(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// �t�@�C���̃��b�V������
	file_mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FileMeshComp"));

	// �E�B�W�F�b�g�R���|�[�l���g����
	widget_comp_ = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));

	if (file_mesh_ != NULL)		file_mesh_->SetupAttachment(RootComponent);
	if (widget_comp_ != NULL)	widget_comp_->SetupAttachment(file_mesh_);
}

void ACanExamineItem::BeginPlay()
{
	// �v���C���[���擾
	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// �v���C���[���擾�ł��Ȃ���΃��b�Z�[�W�\��
	if (player_character_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// �f�[�^�e�[�u�����Z�b�g����Ă���΃f�[�^�e�[�u���̑S�e�L�X�g���擾
	if (datatable_ != NULL)
	{
		TArray<TArray<FString>> datatable_all_text = datatable_->GetTableData();

		// �w�肵���t�@�C���̍ő��܂ōs�����A�������͂���Ă��Ȃ��Z���ɓ�����܂Ńe�L�X�g���擾�A�i�[
		for (int i = 1, datatable_row_length = datatable_all_text[file_kind_].Num(); i < datatable_row_length && datatable_all_text[file_kind_][i] != ""; ++i)
		{
			text_in_file_kind_.Add(datatable_all_text[file_kind_][i]);
		}

		// �L���ȃy�[�W������Α��y�[�W����ݒ�
		if (text_in_file_kind_[0] != "@")
		{
			page_num_ = text_in_file_kind_.Num();
		}
		// ������΍폜
		else	this->Destroy();
	
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("DataTable Not be Found !"));

	// �}�b�v�ɒu����Ă��鎞�̃g�����X�t�H�[�����i�[
	transform_on_map_ = GetActorTransform();

	// �����e�L�X�g�\��
	TurnPage();

	Super::BeginPlay();
}

void ACanExamineItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
					player_character_->SetPlayerControlFlag(true);

					SetActorTransform(transform_on_map_);

					can_show_detial_ = true;
				}
			}
		}
	}
}

// �v���C���[����̃`�F�b�N�����ꂽ(���ׂ�ꂽ)
void ACanExamineItem::CheckedByPlayer()
{
	if (can_show_detial_)
	{
		// �܂����ׂ��Ă��Ȃ��Ȃ�v���C���[�̖ڂ̑O�ɕ\��
		if (is_show_details_ == false)
		{
			// �v���C���[�̑����s�ɂ���
			player_character_->SetPlayerControlFlag(false);

			// ���ׂĂ���t���O�𗧂Ă�
			is_show_details_ = true;

			count_for_time_open_close_ = 0.0f;

			// �v���C���[�̖ڂ̑O�ɕ\��
			SetActorLocation(player_character_->ReturnCameraLocation() + (player_character_->ReturnCameraForwardVector() * distance_from_file_to_player_));
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player_character_->ReturnCameraLocation()));
		}
		else
		{
			// �y�[�W���߂��鋖���o�Ă���΃e�L�X�g�X�V
			if (can_turn_page_)	TurnPage();

			// �e�L�X�g�؂�ւ�
			OnFileTrunPageEventDispatcher.Broadcast();
		}
	}
}

// �y�[�W���߂���
void ACanExamineItem::TurnPage()
{
	// ���̍��y�[�W���L���Ȃ�e�L�X�g�؂�ւ�
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
	else
	{
		//------------------------------------------------
		// �{����ă}�b�v�ɖ߂�
		//------------------------------------------------

		// �e�L�X�g�𓧖���
		text_linear_color_ = FLinearColor(0, 0, 0, 0);

		// �J���Ă���y�[�W�������������X�V
		left_page_open_now_num_ = -1;
		TurnPage();

		// �t�@�C�����J���Ă���t���O���~�낷
		can_start_anim_ = false;

		is_show_details_ = false;

		can_turn_page_ = false;

		can_show_detial_ = false;

		count_for_time_open_close_ = 0.0f;
	}
}