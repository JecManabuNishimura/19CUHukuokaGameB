//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorBody.cpp
// �T�v			�F�����h�A�{�̂𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/04
//-------------------------------------------------------------------

#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

AAutomaticDoorBody::AAutomaticDoorBody()
	: m_pEventTriggerBox(NULL)
	, m_pLeftDoorComp(NULL)
	, m_pRightDoorComp(NULL)
	, lamp_for_decide_pos_(NULL)
	, lever_state_lamp_(NULL)
	, lever_on_state_material_(NULL)
	, lever_off_state_material_(NULL)
	, m_detectSpan(3.0f)
	, m_openAndCloseTime(1.0f)
	, m_leftDoorStartPosY(0.0f)
	, m_leftDoorEndPosY(0.0f)
	, m_rightDoorStartPosY(0.0f)
	, m_doorFilter(0)
	, lamp_generate_pos_(FVector::ZeroVector)
	, m_isSwitchOn(false)
	, m_isOverlap(false)
	, m_openTimeCount(0.0f)
	, m_requiredTime(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_distanceStartToEnd(0.0f)
	, m_detectNum(0)
	, m_doorState(DOOR_STATE_CLOSED)
	, items_Mission_Num(0)
	, next_Items_Mission_Num(0)
	, isMissionComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// ���I�z��̏�����
	filter_match_levers_.Reset();
	match_lever_state_lamps_.Reset();

	// ���m�p�C�x���g�{�b�N�X����
	m_pEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("m_pEventTriggerBox"));
	if (m_pEventTriggerBox != NULL)
	{
		RootComponent = m_pEventTriggerBox;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// ���h�A�̂��ƂƂȂ郁�b�V���쐬
	m_pLeftDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pLeftDoorComp"));
	if (m_pLeftDoorComp != NULL)
	{
		m_pLeftDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pLeftDoorComp has not been created."));

	// �E�h�A�̂��ƂƂȂ郁�b�V���쐬
	m_pRightDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRightDoorComp"));
	if (m_pRightDoorComp != NULL)
	{
		m_pRightDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pRightDoorComp has not been created."));

	lamp_for_decide_pos_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("lamp_for_decide_pos_"));
	if (lamp_for_decide_pos_ != NULL)
	{
		lamp_for_decide_pos_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("lamp_for_decide_pos_ has not been created."));

	// �֐��o�C���h
	m_pEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapBegin);
	m_pEventTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapEnd);
}
AAutomaticDoorBody::~AAutomaticDoorBody()
{
}

void AAutomaticDoorBody::BeginPlay()
{
	Super::BeginPlay();

	// �t�B���^�[�ԍ�����v���郌�o�[�擾�A�ۑ�
	TSubclassOf<AAutomaticDoorLever> findClass;
	findClass = AAutomaticDoorLever::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(actors[idx]);
			// ���o�[�̃t�B���^�[�ԍ����h�A���g�̃t�B���^�[�ԍ��ƈ�v���Ă���Δz��ɒǉ�����
			if (pLever->GetLeverFilter() == m_doorFilter)
			{
				filter_match_levers_.Add(pLever);
			}
		}

		filter_match_levers_num_ = filter_match_levers_.Num();

		// �h�A�ɑΉ����郌�o�[�����x���ɔz�u����Ă���΂��̐������v�𐶐�
		if (filter_match_levers_num_ != 0)
		{
			lamp_generate_pos_ = lamp_for_decide_pos_->GetRelativeLocation();

			FString lamp_name = "lever_state_lamp";

			if (lever_state_lamp_ != NULL)
			{
				for (int i = 0; i < filter_match_levers_num_; ++i)
				{
					match_lever_state_lamps_.Add(NewObject<UStaticMeshComponent>(this));
					match_lever_state_lamps_[i]->RegisterComponent();
					match_lever_state_lamps_[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
					match_lever_state_lamps_[i]->SetStaticMesh(lever_state_lamp_);
					match_lever_state_lamps_[i]->SetRelativeLocation(FVector(lamp_generate_pos_.X, ((30.f * (i + 1)) + (-15.f * (float)filter_match_levers_num_) - 15.f), lamp_generate_pos_.Z));
				}
			}
		}
		// �h�A�ɑΉ����郌�o�[�����x���ɔz�u����Ă��Ȃ��|�����O�o��
		else		
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding lever is not installed on the level !"));
		}
	}
	// �����������o�[��������x���ɔz�u����Ă��Ȃ����߃G���[
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no lever installed on the level !"));
	}

	lamp_for_decide_pos_->DestroyComponent();

	// �h�A�̏����ʒu���擾
	m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	// �h�A�̎n�_����I�_�܂ł̋������v�Z
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	m_leftDoorMoveDirection = FMath::Abs(m_leftDoorEndPosY) / m_leftDoorEndPosY;
}

void AAutomaticDoorBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDoorState();

	UpdateDoorMove(DeltaTime);
}

void AAutomaticDoorBody::UpdateDoorState()
{
	switch (m_doorState)
	{
	case DOOR_STATE_CLOSED:
		if (m_isSwitchOn && m_isOverlap)
		{
			m_doorState = DOOR_STATE_OPENING;
		}
		break;

	case DOOR_STATE_OPENED:
		if (!m_isSwitchOn)
		{
			m_doorState = DOOR_STATE_CLOSING;
		}
		break;

	case DOOR_STATE_CLOSING:
		if (m_isSwitchOn && m_isOverlap)
		{
			m_doorState = DOOR_STATE_OPENING;
		}
		else if (m_requiredTime <= 0.0f)
		{
			m_doorState = DOOR_STATE_CLOSED;
		}
		break;

	case DOOR_STATE_OPENING:
		if (!m_isSwitchOn)
		{
			m_doorState = DOOR_STATE_CLOSING;
		}
		else if (m_requiredTime >= m_openAndCloseTime)
		{
			m_doorState = DOOR_STATE_OPENED;
		}
		break;

	default:
		break;
	}
}

void AAutomaticDoorBody::UpdateDoorMove(float _deltaTime)
{
	switch (m_doorState)
	{
	case DOOR_STATE_OPENED:
		CheckDetectSpan(_deltaTime);
	case DOOR_STATE_CLOSED:
		return;
		break;

	case DOOR_STATE_CLOSING:
		m_openTimeCount = 0.0f;
		m_requiredTime -= _deltaTime;		
		if (m_requiredTime < 0.0f) { m_requiredTime = 0.0f; }
		break;

	case DOOR_STATE_OPENING:
		m_requiredTime += _deltaTime;
		if (m_requiredTime > m_openAndCloseTime) { m_requiredTime = m_openAndCloseTime; }
		break;

	default:
		break;
	}

	// ���h�A�̏���
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void AAutomaticDoorBody::CheckDetectSpan(float _deltaTime)
{
	m_openTimeCount += _deltaTime;

	// ���߂����Ԃ𒴂�����`�F�b�N
	if (m_openTimeCount > m_detectSpan)
	{
		// ���m�ł��Ȃ���Ώ�Ԃ�CLOSING��
		if (!m_isOverlap)
		{
			m_doorState = DOOR_STATE_CLOSING;
		}
		else
		{
			// �J�������p���̂��߂̐��l���
			m_openTimeCount = 0.0f;
		}
	}
}

void AAutomaticDoorBody::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	// ���m����Actor��Player��Enemy��������
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// �͈͓����m�����C���N�������g
		++m_detectNum;

		// ���m�t���O�𗧂Ă�
		m_isOverlap = true;

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
		--m_detectNum;

		// �͈͓��ɒN�����Ȃ��Ȃ猟�m�t���O��������
		if (m_detectNum == 0)
		{
			m_isOverlap = false;
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
					match_lever_state_lamps_[i]->SetMaterial(0, lever_off_state_material_);
					m_isSwitchOn = false;
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
			match_lever_state_lamps_[i]->SetMaterial(0, lever_on_state_material_);
			switch_state = switch_state && true;
		}
		else
		{
			match_lever_state_lamps_[i]->SetMaterial(0, lever_off_state_material_);
			switch_state = switch_state && false;
		}
	}

	// �Ή����郌�o�[�S�Ă̏�Ԃ�ON(=true)�Ȃ猟�m�t���O�𗧂Ă�
	m_isSwitchOn = switch_state;

	// �~�b�V�����ɔ��f����ꍇ�A�X�}�z�̃~�b�V�������A�b�v�f�[�g (�쐬��:�щ_��)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			if (m_isSwitchOn == true)
				GoToPlayerCharacterAndUpdateMission();

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
		bool tempbool = false;
		player1->UpdateTheMission(0, this->next_Items_Mission_Num, tempbool);
	} // end if()
	
} // GoToPlayerCharacterAndUpdateMission()
