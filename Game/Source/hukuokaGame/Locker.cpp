// max_rotation��0�ȏ�ɂ���Ƃ��������Ȃ�܂�
// 181��~360���ɂ���Ƃ��������Ȃ�
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
	, max_rotation(0)
	, open_and_close_frame(0)
	, player_change_rotation_frame(0)
	, player_to_locker_frame(0)
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
	, locker_body_rotation_(FRotator::ZeroRotator)
	, location_lerp_alpha(0.f)
	, location_add_lerp_value(0.f)
	, rotation_lerp_alpha(0.f)
	, rotation_add_lerp_value(0.f)
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

	if (max_rotation == 0 || open_and_close_frame == 0 || player_change_rotation_frame == 0 || player_to_locker_frame == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Propety is 0. Should be Another Value"));
	}
}

void ALocker::BeginPlay()
{
	Super::BeginPlay();

	// 1�t���[�����Ƃɂǂꂾ����]�����邩�̐ݒ�(1�t���[���ɗ^�����]�� = ��]���������ő�̊p�x / ������F��)
	add_rotation_value_ = (float)(max_rotation / open_and_close_frame);

	def_door_rotation = locker_body_rotation_.Yaw;

	locker_body_location_ = body_mesh_->GetRelativeLocation();
	locker_body_rotation_ = body_mesh_->GetRelativeRotation();
	//max_rotation += locker_body_rotation_.Yaw;
	//now_rotation_value_ = locker_body_rotation_.Yaw;
	locker_body_rotation_.Yaw += 90.f;
	//add_player_rotation_value_ = locker_body_rotation_.Yaw / player_change_rotation_frame;

	rotation_add_lerp_value = 1.f / (float)player_change_rotation_frame;

	GetPlayer();
}

void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	is_end_rotation_ = false;
	// ���̊J���E����Ƃ��̊p�x�̒l�̐ݒ�
	SetDoorRotationValue();

	// �p�x�̔��f
	UpdateDoorRotation();

	// �v���C���[�����b�J�[�̒��ɓ���E�o�鏈��
	if (!is_in_player_)
	{
		InToLocker();
	}
	else
	{
		OutToLocker();
	}

	if (player_rotation_start_flag_)
	{
		PlayerRotation();
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

	is_check_ = !is_check_;
	can_input_ = false;
	// �v���C���[�����b�J�[�Ɉړ�����ۂ�1F�ӂ�̈ړ��� = (���b�J�[�̍��W - Player�̌��ݒn) / ���t���[����

	if (!is_in_player_)
	{
		player_location_save_ = player->GetActorLocation();

		location_lerp_alpha = 0.f;
		player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
		location_add_lerp_value = 1.f / (float)player_to_locker_frame;
		locker_body_location_.Z = player_location_save_.Z;

		rotation_lerp_alpha = 0.f;
	}
	else
	{
		location_lerp_alpha = 1.f;
		//player_move_vector_ = (player->GetActorLocation() - player_location_save_) / player_to_locker_frame;
	}

	player->SetPlayerControlFlag(false);
}

void ALocker::SetDoorRotationValue()
{
	// ���b�J�[�̔����J����t���O������
	if (is_check_)
	{
		// �J�������Ă��Ȃ����ǂ���
		if (now_rotation_value_ >= max_rotation)
		{
			now_rotation_value_ += add_rotation_value_;

			// max_rotation�𒴂��Ă���Ȃ�␳(�����v��Ȃ�����)
			if (now_rotation_value_ <= max_rotation)
			{
				now_rotation_value_ = max_rotation;
				is_end_rotation_ = true;
			}
		}
	}
	else
	{
		// �܂�؂��Ă��Ȃ����ǂ���
		if (now_rotation_value_ < 0)
		{
			now_rotation_value_ -= add_rotation_value_;

			// 0�����Ȃ�␳(�����v��Ȃ�����)
			if (now_rotation_value_ >= 0)
			{
				now_rotation_value_ = 0.f;
				// �܂�Ƃ��͗v��Ȃ�
				//is_end_rotation_ = true;			
			}
		}
	}
}

void ALocker::UpdateDoorRotation()
{
	door_mesh_->SetRelativeRotation(FRotator(0.f, now_rotation_value_, 0.f));
}

void ALocker::InToLocker()
{
	// ������]���I���Ă��Ȃ� ���� ��]�������I���������Ă��Ȃ��Ȃ珈�������Ȃ�
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Player�̈ړ����̈ړ����Ȉ�true��
	is_move_in_locker_ = true;

	location_lerp_alpha += location_add_lerp_value; 
	player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
	player->SetActorLocation(player_move_vector_);
	
	if (location_lerp_alpha >= 0.99f)
	{
		player_rotation_start_flag_ = true;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = true;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
}

void ALocker::OutToLocker()
{
	// ������]���I���Ă��Ȃ� ���� ��]�������I���������Ă��Ȃ��Ȃ珈�������Ȃ�
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Player�̈ړ����̈ړ����Ȉ�true��
	is_move_in_locker_ = true;

	location_lerp_alpha -= location_add_lerp_value;
	player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
	player->SetActorLocation(player_move_vector_);

	if(location_lerp_alpha <= 0)
	{
		player_rotation_start_flag_ = false;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = false;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
}

void ALocker::PlayerRotation()
{
		
	rotation_lerp_alpha += rotation_add_lerp_value;
	player->SetActorRotation(FMath::Lerp(player->GetActorRotation(), locker_body_rotation_, rotation_lerp_alpha));

	if (rotation_lerp_alpha >= 0.99f)
	{
		player_rotation_start_flag_ = false;
	}
}

void ALocker::CloseDoor()
{

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