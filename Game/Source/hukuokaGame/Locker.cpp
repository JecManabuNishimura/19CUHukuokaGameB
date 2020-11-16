// max_rotation��0�ȏ�ɂ���Ƃ��������Ȃ�܂�

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

	locker_body_location_ = body_mesh_->GetRelativeLocation();
	locker_body_rotation_ = body_mesh_->GetRelativeRotation();
	locker_body_rotation_.Yaw += 90.f;
	add_player_rotation_value_ = locker_body_rotation_.Yaw / player_change_rotation_frame;

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
		player_move_vector_ = (locker_body_location_ - player_location_save_) / player_to_locker_frame;
	}
	else
	{
		player_move_vector_ = (player->GetActorLocation() - player_location_save_) / player_to_locker_frame;
	}

	player->SetPlayerControlFlag(false);
}

void ALocker::SetDoorRotationValue()
{
	// ���b�J�[�̔����J����t���O������
	if (is_check_)
	{
		// �J�������Ă��Ȃ����ǂ���
		if (door_mesh_->GetRelativeRotation().Yaw >= max_rotation)
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
		if (door_mesh_->GetRelativeRotation().Yaw < 0)
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
	door_mesh_->SetWorldRotation(FRotator(0.f, now_rotation_value_, 0.f));
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

	// �v���C���[�̎��ɂ���ׂ����ݒn = �v���C���[�̌��ݒn + 1�t���[���ӂ�Ɉړ�������x�N�^�[
	FVector playerNowLocation = player->GetActorLocation() + player_move_vector_;
	player->SetActorLocation(playerNowLocation);

	// Z��0
	player->SetActorLocation(FVector(playerNowLocation.X, playerNowLocation.Y, player_location_save_.Z));

	// ��Βl�ł�肽�������B
	// float absPlayerLocation_X = fabs(playerNowLocation.X);
	// float absPlayerLocation_Y = fabs(playerNowLocation.Y);
	float abslockerBodyLocation_X = locker_body_location_.X;
	float abslockerBodyLocation_Y = locker_body_location_.Y;

	if (locker_body_location_.X < 0)
	{
		abslockerBodyLocation_X = locker_body_location_.X * -1;
	}
	if (locker_body_location_.Y < 0)
	{
		abslockerBodyLocation_Y = locker_body_location_.Y * -1;
	}


	if (playerNowLocation.Y + abslockerBodyLocation_Y < 0 && playerNowLocation.X + abslockerBodyLocation_Y < 0)
	{
		player_rotation_start_flag_ = true;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = true;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
	//player->SetActorRotation(locker_body_rotation_);
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

	// �v���C���[�̎��ɂ���ׂ����ݒn = �v���C���[�̌��ݒn + 1�t���[���ӂ�Ɉړ�������x�N�^�[
	FVector nowPlayerLocation = player->GetActorLocation() - player_move_vector_;
	player->SetActorLocation(nowPlayerLocation);

	// Z���␳
	player->SetActorLocation(FVector(nowPlayerLocation.X, nowPlayerLocation.Y, player_location_save_.Z));

	// ���̈ʒu�ɕt������
	float a = nowPlayerLocation.X - player_location_save_.X;
	float b = nowPlayerLocation.Y - player_location_save_.Y;

	//UE_LOG(LogTemp, Warning, TEXT("x : %f - %f = %f"), nowPlayerLocation.X, player_location_save_.X, a);
	//UE_LOG(LogTemp, Warning, TEXT("f : %f - %f = %f"), nowPlayerLocation.Y, player_location_save_.Y, b);
	//UE_LOG(LogTemp, Warning, TEXT("-----------------------------------------------------"));

	if(a > -5 && b > -5)
	{
		player_rotation_start_flag_ = true;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = false;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
	//player->SetActorRotation(locker_body_rotation_);
}

void ALocker::PlayerRotation()
{
	FRotator playerNowRotation = player->GetActorRotation();
	playerNowRotation.Yaw += add_player_rotation_value_;
	player->SetActorRotation(playerNowRotation);

	if (playerNowRotation.Yaw >= locker_body_rotation_.Yaw)
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