//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorBody.cpp
// �T�v			�F�����h�A�{�̂𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/04
//-------------------------------------------------------------------

#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

AAutomaticDoorBody::AAutomaticDoorBody()
	: p_eventTriggerBox_(NULL)
	, p_left_door_comp_(NULL)
	, p_right_door_comp_(NULL)
	, p_lamp_for_decide_pos_(NULL)
	, p_lever_state_lamp_(NULL)
	, p_lever_on_state_material_(NULL)
	, p_lever_off_state_material_(NULL)
	, p_sound_door_open_(NULL)
	, p_sound_door_close_(NULL)
	, detect_span_(3.0f)
	, open_and_close_time_(1.0f)
	, left_door_start_posY_(0.0f)
	, left_door_end_posY_(0.0f)
	, right_door_start_posY_(0.0f)
	, door_filter_num_(0)
	, lamp_generate_pos_(FVector::ZeroVector)
	, is_switch_on_(false)
	, is_overlap_(false)
	, open_time_count_(0.0f)
	, required_time_for_open_and_close_(0.0f)
	, left_door_dire_for_move_(1.0f)
	, distance_start_to_end_(0.0f)
	, detect_num_(0)
	, door_state_(kDoorStateClosed)
	, p_alert_lever_(NULL)
	, is_alert_lever_on_(false)
	, items_Mission_Num(0)
	, next_Items_Mission_Num(0)
	, isMissionComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// ���I�z��̏�����
	filter_match_levers_.Reset();
	match_lever_state_lamps_.Reset();

	// ���m�p�C�x���g�{�b�N�X����
	p_eventTriggerBox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("p_eventTriggerBox_"));
	if (p_eventTriggerBox_ != NULL)
	{
		RootComponent = p_eventTriggerBox_;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// ���h�A�̂��ƂƂȂ郁�b�V���쐬
	p_left_door_comp_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_left_door_comp_"));
	if (p_left_door_comp_ != NULL)
	{
		p_left_door_comp_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_left_door_comp_ has not been created."));

	// �E�h�A�̂��ƂƂȂ郁�b�V���쐬
	p_right_door_comp_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_right_door_comp_"));
	if (p_right_door_comp_ != NULL)
	{
		p_right_door_comp_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_right_door_comp_ has not been created."));

	p_lamp_for_decide_pos_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_lamp_for_decide_pos_"));
	if (p_lamp_for_decide_pos_ != NULL)
	{
		p_lamp_for_decide_pos_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_lamp_for_decide_pos_ has not been created."));

	// �֐��o�C���h
	p_eventTriggerBox_->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapBegin);
	p_eventTriggerBox_->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapEnd);
}
AAutomaticDoorBody::~AAutomaticDoorBody()
{
}

void AAutomaticDoorBody::BeginPlay()
{
	Super::BeginPlay();

	// �t�B���^�[�ԍ�����v���郌�o�[�Ɣ�탌�o�[�擾�A�ۑ�
	TSubclassOf<AAutomaticDoorLever> findClass;
	findClass = AAutomaticDoorLever::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorLever* p_lever = Cast<AAutomaticDoorLever>(actors[idx]);

			// ���o�[�̃t�B���^�[�ԍ����h�A���g�̃t�B���^�[�ԍ��ƈ�v���Ă���Δz��ɒǉ�����
			if (p_lever->GetLeverFilter() == door_filter_num_)
			{
				filter_match_levers_.Add(p_lever);
			}

			// ��탌�o�[�Ȃ�ۑ�
			if (p_lever->ActorHasTag("AlertLever"))
			{
				p_alert_lever_ = p_lever;
			}
		}

		filter_match_levers_num_ = filter_match_levers_.Num();

		// �h�A�ɑΉ����郌�o�[�����x���ɔz�u����Ă���΂��̐������v�𐶐�
		if (filter_match_levers_num_ != 0)
		{
			lamp_generate_pos_ = p_lamp_for_decide_pos_->GetRelativeLocation();

			FString lamp_name = "lever_state_lamp";

			if (p_lever_state_lamp_ != NULL)
			{
				for (int i = 0; i < filter_match_levers_num_; ++i)
				{
					match_lever_state_lamps_.Add(NewObject<UStaticMeshComponent>(this));
					match_lever_state_lamps_[i]->RegisterComponent();
					match_lever_state_lamps_[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
					match_lever_state_lamps_[i]->SetStaticMesh(p_lever_state_lamp_);
					match_lever_state_lamps_[i]->SetRelativeLocation(FVector(lamp_generate_pos_.X, ((((40.f * (i + 1)) + (-20.f * (float)(filter_match_levers_num_ + 1))) * p_lamp_for_decide_pos_->GetRelativeScale3D().X) + lamp_generate_pos_.Y), lamp_generate_pos_.Z));
					match_lever_state_lamps_[i]->SetRelativeScale3D(p_lamp_for_decide_pos_->GetRelativeScale3D());
				}
			}
		}
	}

	p_lamp_for_decide_pos_->DestroyComponent();

	// �h�A�̏����ʒu���擾
	left_door_start_posY_ = p_left_door_comp_->GetRelativeLocation().Y;
	right_door_start_posY_ = p_right_door_comp_->GetRelativeLocation().Y;

	// �h�A�̎n�_����I�_�܂ł̋������v�Z
	distance_start_to_end_ = FMath::Abs(left_door_end_posY_ - left_door_start_posY_);

	// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	left_door_dire_for_move_ = FMath::Abs(left_door_end_posY_) / left_door_end_posY_;
}

void AAutomaticDoorBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAlertLeverState();

	UpdateDoorState();

	UpdateDoorMove(DeltaTime);
}

void AAutomaticDoorBody::CheckAlertLeverState()
{
	if (is_alert_lever_on_)	return;

	if (p_alert_lever_ != NULL)
	{
		is_alert_lever_on_ = p_alert_lever_->GetLeverState();

		if (is_alert_lever_on_)
		{
			is_switch_on_ = true;
			door_state_ = kDoorStateOpening;
		}
	}
}

void AAutomaticDoorBody::UpdateDoorState()
{
	switch (door_state_)
	{
	case kDoorStateClosed:
		if (is_switch_on_ && is_overlap_)
		{
			door_state_ = kDoorStateOpening;

			// �h�A���J������SE��炷
			if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
		}
		break;

	case kDoorStateOpened:
		if (!is_switch_on_ && !is_alert_lever_on_)
		{
			door_state_ = kDoorStateClosing;
		}
		break;

	case kDoorStateClosing:
		if (is_switch_on_ && is_overlap_)
		{
			door_state_ = kDoorStateOpening;

			// �h�A���J������SE��炷
			if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
		}
		else if (required_time_for_open_and_close_ <= 0.0f)
		{
			door_state_ = kDoorStateClosed;
		}
		break;

	case kDoorStateOpening:
		if (!is_switch_on_ && !is_alert_lever_on_)
		{
			door_state_ = kDoorStateClosing;
		}
		else if (required_time_for_open_and_close_ >= open_and_close_time_)
		{
			door_state_ = kDoorStateOpened;
		}
		break;

	default:
		break;
	}
}

void AAutomaticDoorBody::UpdateDoorMove(float _deltaTime)
{
	switch (door_state_)
	{
	case kDoorStateOpened:
		CheckDetectSpan(_deltaTime);
	case kDoorStateClosed:
		return;
		break;

	case kDoorStateClosing:
		open_time_count_ = 0.0f;
		required_time_for_open_and_close_ -= _deltaTime;		
		if (required_time_for_open_and_close_ < 0.0f) { required_time_for_open_and_close_ = 0.0f; }
		break;

	case kDoorStateOpening:
		required_time_for_open_and_close_ += _deltaTime;
		if (required_time_for_open_and_close_ > open_and_close_time_) { required_time_for_open_and_close_ = open_and_close_time_; }
		break;

	default:
		break;
	}

	// ���h�A�̏���
	float newLocationY = left_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_);
	p_left_door_comp_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = right_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_ * -1.0f);
	p_right_door_comp_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void AAutomaticDoorBody::CheckDetectSpan(float _deltaTime)
{
	if (is_alert_lever_on_)	return;

	open_time_count_ += _deltaTime;

	// ���߂����Ԃ𒴂�����`�F�b�N
	if (open_time_count_ > detect_span_)
	{
		// ���m�ł��Ȃ���Ώ�Ԃ�CLOSING��
		if (!is_overlap_)
		{
			door_state_ = kDoorStateClosing;

			// �h�A�����鎞��SE��炷
			if (p_sound_door_close_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_close_, GetActorLocation());
		}
		else
		{
			// �J�������p���̂��߂̐��l���
			open_time_count_ = 0.0f;
		}
	}
}

void AAutomaticDoorBody::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	// ���m����Actor��Player��Enemy��������
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// �͈͓����m�����C���N�������g
		++detect_num_;

		// ���m�t���O�𗧂Ă�
		is_overlap_ = true;

		// �h�A�̍쓮�t���O�̍X�V
		UpdateSwitchState();
	}
}

void AAutomaticDoorBody::OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	// ���m����Actor��Player��Enemy��������
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// �͈͓����m�����f�N�������g
		--detect_num_;

		// �͈͓��ɒN�����Ȃ��Ȃ猟�m�t���O��������
		if (detect_num_ == 0)
		{
			is_overlap_ = false;
		}
	}
}

// �h�A�{�̂̃X�C�b�`�̍X�V
void AAutomaticDoorBody::UpdateSwitchState(const AAutomaticDoorLever* const operated_lever)
{
	// ���o�[������Ăяo����Ȃ������̏�Ԃ�OFF�������ꍇ
	if (operated_lever != nullptr)
	{
		for (int i = 0; i < filter_match_levers_num_; ++i)
		{
			if (filter_match_levers_[i] == operated_lever)
			{
				if (filter_match_levers_[i]->GetLeverState() == false)
				{
					match_lever_state_lamps_[i]->SetMaterial(0, p_lever_off_state_material_);
					is_switch_on_ = false;
					return;
				}
				break;
			}
		}
	}

	bool switch_state = true;

	// �h�A�{�̂���Ăяo���ꂽ�ꍇ����ё��삳�ꂽ���o�[��ON�������ꍇ
	for (int i = 0; i < filter_match_levers_num_; ++i)
	{
		if (filter_match_levers_[i]->GetLeverState() == true)
		{
			match_lever_state_lamps_[i]->SetMaterial(0, p_lever_on_state_material_);
			switch_state = switch_state && true;
		}
		else
		{
			match_lever_state_lamps_[i]->SetMaterial(0, p_lever_off_state_material_);
			switch_state = switch_state && false;
		}
	}
	
	// �Ή����郌�o�[�S�Ă̏�Ԃ�ON(=true)�Ȃ猟�m�t���O�𗧂Ă�
	is_switch_on_ = switch_state;

	// �~�b�V�����ɔ��f����ꍇ�A�X�}�z�̃~�b�V�������A�b�v�f�[�g (�쐬��:�щ_��)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			if (is_switch_on_ == true) {
				GoToPlayerCharacterAndUpdateMission();
			} // end if()
		} // end if()
	} // end if()
}

// �v���C���[��UpdateTheMission���Ăяo�����߂̃��]�b�g (�쐬��:�щ_��)
void AAutomaticDoorBody::GoToPlayerCharacterAndUpdateMission()
{
	APlayerCharacter* player1 = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	player1->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);

	if (this->next_Items_Mission_Num != 0)
	{
		isMissionComplete = true;
		bool tempbool = false;
		player1->UpdateTheMission(0, this->next_Items_Mission_Num, tempbool);
	} // end if()
	
} // GoToPlayerCharacterAndUpdateMission()
