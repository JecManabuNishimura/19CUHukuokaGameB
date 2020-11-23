// max_rotation_ > 0�ɂ��Ȃ�
// �z�u�̍ۂ�BP_Locker��u������
//-------------------------------------------------------------------
// �t�@�C��		�FLocker.cpp
// �T�v			�F���b�J�[�̐���
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/11/16
//-------------------------------------------------------------------

#include "Locker.h"

ALocker::ALocker()
	: body_mesh_(NULL)
	, door_mesh_(NULL)
	, max_rotation_(0)
	, open_and_close_second_(0.f)
	, player_change_rotation_second_(0.f)
	, player_to_locker_second_(0.f)
	, flont_distance_(0)
	, player(NULL)
	, is_end_rotation_(false)
	, can_input_(true)
	, is_check_(false)
	, is_move_in_locker_(false)
	, is_in_player_(false)
	, player_rotation_start_flag_(false)
	, add_rotation_value_(0.f)
	, now_rotation_value_(0.f)
	, add_player_rotation_value_(0.f)
	, locker_body_location_(FVector::ZeroVector)
	, player_location_save_(FVector::ZeroVector)
	, player_move_vector_(FVector::ZeroVector)
	, body_front_location_(FVector::ZeroVector)
	, locker_body_rotation_(FRotator::ZeroRotator)
	, location_lerp_alpha1_(0.f)
	, location_lerp_alpha2_(0.f)
	, location_add_lerp_value_(0.f)
	, rotation_lerp_alpha_(0.f)
	, rotation_add_lerp_value_(0.f)
{
	// �e�B�b�N���Ăяo�����̃t���O
	PrimaryActorTick.bCanEverTick = true;

	// �e���b�V���̐ݒ�(�R���|�[�l���g�Ƃ��Ēǉ�)
	body_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("body_mesh_"));
	if (body_mesh_ != NULL)
	{
		RootComponent = body_mesh_;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Locker.cpp body_mesh_ is NULL"));
	}

	door_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("door_mesh_"));
	if (door_mesh_ != NULL)
	{
		door_mesh_->SetupAttachment(body_mesh_);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Locker.cpp door_mesh_ is NULL"));
	}
}

void ALocker::BeginPlay()
{
	Super::BeginPlay();

	// 1�b���Ƃɂǂꂾ����]�����邩�̐ݒ�(1�b�ɗ^�����]�� = ��]���������ő�̊p�x / ������b��)
	add_rotation_value_ = (float)(max_rotation_) / open_and_close_second_;

	// �N�����̃��b�J�[�{�̂�Z�����i�[
	def_door_rotation_ = locker_body_rotation_.Yaw;

	// player_location_save_->door_front_location �� body_front_location_->locker_body_location_�ňړ�������ׁA2�Ŋ���
	location_add_lerp_value_ = 2.f / (float)player_to_locker_second_;

	// ���b�J�[�{�̂̍��W�E�p�x���i�[���A�p�x�ɂ�Z���̂�+90����(�łȂ��ƃ��b�J�[��Player���������ۂɈӐ}���Ȃ���������������)
	locker_body_location_ = body_mesh_->GetRelativeLocation();
	locker_body_rotation_ = body_mesh_->GetRelativeRotation();
	locker_body_rotation_.Yaw += 90.f;

	// ���b�J�[�̐��ʂ̍��W���i�[(flont_distance_�Œ�����)
	body_front_location_ = GetActorRightVector() * flont_distance_ + GetActorLocation();

	// 1F�ӂ�ɉ��Z����l = 1 / �v���C���[�����b�J�[�̒��ŉ�]���鑍�b��
	// 1 = (lerp�̍ő�l)
	rotation_add_lerp_value_ = 1.f / player_change_rotation_second_;

	GetPlayer();
}

void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ���̉�]�I���t���O�̏�����
	is_end_rotation_ = false;

	// ���̊J���E����Ƃ��̊p�x�̒l�̐ݒ�
	SetDoorRotationValue(DeltaTime);

	// �p�x�̔��f
	UpdateDoorRotation(DeltaTime);

	// �v���C���[�����b�J�[�̒��ɓ���E�o�鏈��
	if (!is_in_player_)
	{
		// ���ɓ���
		InToLocker(DeltaTime);
	}
	else
	{
		// �o��
		OutToLocker(DeltaTime);
	}

	// �v���C���[�����b�J�[�̒��ɓ����ăt���O�����Ă�ꂽ��
	if (player_rotation_start_flag_)
	{
		PlayerRotation(DeltaTime);
	}
}

ALocker::~ALocker()
{

}

void ALocker::CheckedByPlayer()
{
	// ���͉\��Ԃ�
	if (!can_input_)
	{
		return;
	}

	// ���͉\�Ȉ׃`�F�b�N��Ԃ̔��]�ƃ��b�J�[�ɑ΂�����͎󂯕t���𖳌���
	is_check_ = !is_check_;
	can_input_ = false;

	// �{�^���������ꂽ�ۂɃv���C���[�����b�J�[�̒��ɂ��邩�ǂ���
	if (!is_in_player_)
	{
		// ���݈ʒu�̕ۑ�
		player_location_save_ = player->GetActorLocation();

		// lerp�Ɏg���l�̏�����
		location_lerp_alpha1_ = 0.f;		// player_location_save_ -> body_front_location_�Ɏg�� leap
		location_lerp_alpha2_ = 0.f;		// body_front_location_  -> locker_body_location_�Ɏg��lerp
		rotation_lerp_alpha_ = 0.f;		// ���b�J�[�ɓ������ۂ̉�]�Ɏg���Ă�lerp

		//���b�J�[�{�̂ƃ��b�J�[�̑O�ɂ�����W��Z����player��Z���ɌŒ�(Begin�ł������̂�������Ȃ�)
		locker_body_location_.Z = player_location_save_.Z;
		body_front_location_.Z = player_location_save_.Z;

		// ���ɓ���t���O�𗧂Ă�
		// ������Player�̐���ɐ��������Ă�
		player->SetInTheLocker(true);
		player->SetPlayerControlFlag(false);
	}
	else
	{
		// lerp�Ɏg���l���ő�l�ŏ�����
		location_lerp_alpha1_ = 1.f;
		location_lerp_alpha2_ = 1.f;
	}
}

void ALocker::SetDoorRotationValue(float DeltaTime)
{
	// ���b�J�[�̔����J����t���O������
	if (is_check_)
	{
		// �J�������Ă��Ȃ����ǂ���(�s��������񂩂�)
		if (now_rotation_value_ >= max_rotation_)
		{
			now_rotation_value_ += add_rotation_value_ * DeltaTime;

			// max_rotation_�𒴂��Ă���Ȃ�␳�E��]�I���t���O�𗧂Ă�
			if (now_rotation_value_ <= max_rotation_)
			{
				now_rotation_value_ = max_rotation_;
				is_end_rotation_ = true;
			}
		}
	}
	else
	{
		// �܂�؂��Ă��Ȃ����ǂ���
		if (now_rotation_value_ < 0)
		{
			now_rotation_value_ -= add_rotation_value_ * DeltaTime;

			// 0�����Ȃ�␳�E��]�I���t���O�𗧂Ă�
			if (now_rotation_value_ >= 0)
			{
				now_rotation_value_ = 0.f;
				// �܂�Ƃ��͗v��Ȃ�(�ړ����Ă��܂���)
				//is_end_rotation_ = true;			
			}
		}
	}
}

void ALocker::UpdateDoorRotation(float DeltaTime)
{
	// SetDoorRotationValue()�Őݒ肵���l�𔽉f
	door_mesh_->SetRelativeRotation(FRotator(0.f, now_rotation_value_, 0.f));
}

void ALocker::InToLocker(float DeltaTime)
{
	// ������]���I���Ă��Ȃ� ���� ��]�������I���������Ă��Ȃ��Ȃ珈�������Ȃ�
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Player�̈ړ����̈ړ����Ȉ�true��
	is_move_in_locker_ = true;

	// player_location_save_ -> body_front_location_ �̈ړ���(�ق�)�I�������
	if (location_lerp_alpha1_ >= 0.99f)
	{
		// body_front_location_  -> locker_body_location_ �̈ړ�(lerp��alpha�l�̐ݒ�)
		location_lerp_alpha2_ += location_add_lerp_value_ * DeltaTime;

		// Player�̍��W�̍X�V
		player_move_vector_ = FMath::Lerp(body_front_location_, locker_body_location_, location_lerp_alpha2_);
		player->SetActorLocation(player_move_vector_);

		// body_front_location_  -> locker_body_location_ �̈ړ���(�ق�)�I�������
		if (location_lerp_alpha2_ >= 0.99f)
		{
			player_rotation_start_flag_ = true;		// Player�̉�]�����̃t���O�𗧂Ă�
			is_move_in_locker_ = false;				// ���b�J�[�̈ړ����I�������̂�false
			is_check_ = false;						// ���ׂ���Ԃ̉���
			is_in_player_ = true;					// ���b�J�[�̒��ɂ���̂�true
			//player->SetPlayerControlFlag(true);		// ����񂩂�
			can_input_ = true;						// ���͎󂯕t�����\��
		}
	}
	// player_location_save_ -> body_front_location_ �̈ړ����I����ĂȂ�
	else
	{
		// player_location_save_->body_front_location_ �̈ړ�(lerp��alpha�l�̐ݒ�)
		location_lerp_alpha1_ += location_add_lerp_value_ * DeltaTime;

		// Player�̍��W�̍X�V
		player_move_vector_ = FMath::Lerp(player_location_save_, body_front_location_, location_lerp_alpha1_);

		player->SetActorLocation(player_move_vector_);
	}
}

void ALocker::OutToLocker(float DeltaTime)
{
	// ������]���I���Ă��Ȃ� ���� ��]�������I���������Ă��Ȃ��Ȃ珈�������Ȃ�
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Player�̈ړ����̈ړ����Ȉ�true��
	is_move_in_locker_ = true;

	// body_front_location_  -> locker_body_location_ �̈ړ�(lerp��alpha�l�̐ݒ�)
	location_lerp_alpha2_ -= location_add_lerp_value_ * DeltaTime;

	// Player�̍��W�̍X�V
	player_move_vector_ = FMath::Lerp(body_front_location_, locker_body_location_, location_lerp_alpha2_);
	player->SetActorLocation(player_move_vector_);

	// body_front_location_  -> locker_body_location_ �̈ړ���(�ق�)�I�������
	if(location_lerp_alpha2_ <= 0)
	{
		player_rotation_start_flag_ = false;	// Player�̉�]�����͂����Ȃ���false
		is_move_in_locker_ = false;				// ���b�J�[�̈ړ����I�������̂�false
		is_check_ = false;						// ���ׂ���Ԃ̉���
		is_in_player_ = false;					// ���b�J�[�̒��ɂ��Ȃ��̂�false
		can_input_ = true;						// ���͎󂯕t�����\��
		player->SetInTheLocker(false);			// Player�����b�J�[�̒��ɂ��Ȃ���Ԃ�
		player->SetPlayerControlFlag(true);
	}
}

void ALocker::PlayerRotation(float DeltaTime)
{
	// Player�̌��݂̊p�x -> locker_body_rotation_�։�]
	rotation_lerp_alpha_ += rotation_add_lerp_value_* DeltaTime;
	player->SetActorRotation(FMath::Lerp(player->GetActorRotation(), locker_body_rotation_, rotation_lerp_alpha_));

	// ��]���I������
	if (rotation_lerp_alpha_ >= 0.99f)
	{
		// ��]�����Ȃ��悤��
		player_rotation_start_flag_ = false;
	}
}

void ALocker::GetPlayer()
{
	TSubclassOf<APlayerCharacter> findClass = APlayerCharacter::StaticClass();
	TArray<AActor*> pPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, pPlayers);
	if (pPlayers.Num())
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(pPlayers[0]);
		FString message = FString("PlayerName") + pPlayer->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, message);

		player = pPlayer;
	}
}