//-------------------------------------------------------------------
// �t�@�C��		�FCardkeyDoorBody.cpp
// �T�v			�F�J�[�h�L�[�Ή��̃h�A�𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/22
//-------------------------------------------------------------------

#include "CardkeyDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

ACardkeyDoorBody::ACardkeyDoorBody()
	: door_body_eventtriggerbox_(NULL)
	, cardreader_mesh_1_(NULL)
	, cardreader_mesh_2_(NULL)
	, leftdoor_mesh_(NULL)
	, rightdoor_mesh_(NULL)
	, door_state_mesh_1_(NULL)
	, door_state_mesh_2_(NULL)
	, door_state_material(NULL)
	, material_instance_dynamic_(NULL)
	, sound_loading_success(NULL)
	, sound_loading_error(NULL)
	, player_character_(NULL)
	, m_detectSpan(3.0f)
	, m_openAndCloseTime(1.0f)
	, check_cardkey_time_(2.0f)
	, display_error_time_(3.0f)
	, count_for_check_display_time_(0.0f)
	, m_leftDoorStartPosY(0.0f)
	, m_leftDoorEndPosY(0.0f)
	, m_rightDoorStartPosY(0.0f)
	, m_doorFilter(0)
	, m_doorState(DOOR_STATE_CLOSED)
	, is_doorbody_eventbox_overlap(false)
	, door_eventbox_overlap_sum(0)
	, m_openTimeCount(0.0f)
	, m_requiredTime(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_distanceStartToEnd(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Root�pSceneComponent����
	USceneComponent* scene_comp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// �h�A�{�̂̌��m�g���K�[�{�b�N�X����
	door_body_eventtriggerbox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("door_body_eventtriggerbox_"));

	// �J�[�h���[�_�[�̃��b�V������1�쐬
	cardreader_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_1"));

	// �J�[�h���[�_�[�̃��b�V������2�쐬
	cardreader_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_2"));

	// ���h�A�̃��b�V���쐬
	leftdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));

	// �E�h�A�̃��b�V���쐬
	rightdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_Parent"));

	// ���h�A�̃��b�N��ԃ��b�V���쐬
	door_state_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorStateMesh"));

	// �E�h�A�̃��b�N��ԃ��b�V���쐬
	door_state_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorStateMesh"));

	door_state_material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/Materials/CardReaderStateMaterial")));
	
	if (scene_comp != NULL)				RootComponent = scene_comp;
	if (leftdoor_mesh_ != NULL)			leftdoor_mesh_->SetupAttachment(RootComponent);
	if (rightdoor_mesh_ != NULL)		rightdoor_mesh_->SetupAttachment(RootComponent);

	if (cardreader_mesh_1_ != NULL)		cardreader_mesh_1_->SetupAttachment(RootComponent);

	if (cardreader_mesh_2_ != NULL)		cardreader_mesh_2_->SetupAttachment(RootComponent);

	if (door_state_mesh_1_ != NULL)		door_state_mesh_1_->SetupAttachment(cardreader_mesh_1_);

	if (door_state_mesh_2_ != NULL)		door_state_mesh_2_->SetupAttachment(cardreader_mesh_2_);

	if (door_body_eventtriggerbox_ != NULL)
	{
		door_body_eventtriggerbox_->SetupAttachment(RootComponent);

		door_body_eventtriggerbox_->OnComponentBeginOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapBegin);
		door_body_eventtriggerbox_->OnComponentEndOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapEnd);
	}
}
ACardkeyDoorBody::~ACardkeyDoorBody()
{
}

void ACardkeyDoorBody::BeginPlay()
{
	Super::BeginPlay();

	if (door_state_material != NULL)
	{
		material_instance_dynamic_ = door_state_mesh_1_->CreateAndSetMaterialInstanceDynamicFromMaterial(0, door_state_material);
		if (material_instance_dynamic_ != NULL)
		{
			door_state_mesh_1_->SetMaterial(0, material_instance_dynamic_);
			door_state_mesh_2_->SetMaterial(0, material_instance_dynamic_);
			material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
		}
		else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("material_instance_dynamic_ is NULL."));
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Door_state_material is NULL."));	

	// �h�A�̏����ʒu���擾
	m_leftDoorStartPosY = leftdoor_mesh_->GetRelativeLocation().Y;
	m_rightDoorStartPosY = rightdoor_mesh_->GetRelativeLocation().Y;

	// �h�A�̎n�_����I�_�܂ł̋������v�Z
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	m_leftDoorMoveDirection = FMath::Abs(m_leftDoorEndPosY) / m_leftDoorEndPosY;

	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player_character_ == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerCharacter is None."));
	}
}

void ACardkeyDoorBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDoorState(DeltaTime);

	UpdateDoorMove(DeltaTime);
}

void ACardkeyDoorBody::UpdateDoorState(float _deltatime)
{
	switch (m_doorState)
	{
	case DOOR_STATE_LOADING:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= check_cardkey_time_)
		{			
			count_for_check_display_time_ = 0.0f;
			if ((player_character_->player_state & (1 << m_doorFilter)) != 0)
			{
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), success_state_color);

				if (sound_loading_success != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_loading_success, GetActorLocation(), 1.0f, 1.0f, 0.0f);
				}
				m_doorState = DOOR_STATE_OPENING;
			}
			else
			{
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), error_state_color);

				if (sound_loading_error != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_loading_error, GetActorLocation(), 1.0f, 1.0f, 0.0f);
				}
				m_doorState = DOOR_STATE_ERROR;
			}
		}
		break;

	case DOOR_STATE_ERROR:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= display_error_time_)
		{
			count_for_check_display_time_ = 0.0f;
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
				m_doorState = DOOR_STATE_CLOSED;
			
		}
		break;

	case DOOR_STATE_OPENED:
		CheckDetectSpan(_deltatime);
		break;

	case DOOR_STATE_CLOSED:
		break;

	case DOOR_STATE_CLOSING:
		if (is_doorbody_eventbox_overlap)
		{
			m_doorState = DOOR_STATE_OPENING;
		}

		if (m_requiredTime <= 0.0f)
		{
			m_doorState = DOOR_STATE_CLOSED;
		}
		break;

	case DOOR_STATE_OPENING:
		if (m_requiredTime >= m_openAndCloseTime)
		{
			m_doorState = DOOR_STATE_OPENED;
		}
		break;

	default:
		break;
	}
}

void ACardkeyDoorBody::UpdateDoorMove(float _deltatime)
{
	switch (m_doorState)
	{
	case DOOR_STATE_OPENED:
	case DOOR_STATE_LOADING:
	case DOOR_STATE_ERROR:
	case DOOR_STATE_CLOSED:
		return;
		break;

	case DOOR_STATE_CLOSING:
		m_openTimeCount = 0.0f;
		m_requiredTime -= _deltatime;
		if (m_requiredTime < 0.0f) { m_requiredTime = 0.0f; }
		break;

	case DOOR_STATE_OPENING:
		m_requiredTime += _deltatime;
		if (m_requiredTime > m_openAndCloseTime) { m_requiredTime = m_openAndCloseTime; }
		break;

	default:
		return;
		break;
	}

	// ���h�A�̏���
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection);
	leftdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection * -1.0f);
	rightdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void ACardkeyDoorBody::CheckDetectSpan(float _deltatime)
{
	m_openTimeCount += _deltatime;

	// ���߂����Ԃ𒴂�����`�F�b�N
	if (m_openTimeCount > m_detectSpan)
	{
		// ���m�ł��Ȃ���Ώ�Ԃ�CLOSING��
		if (is_doorbody_eventbox_overlap)
		{
			// �J�������p���̂��߂̐��l���
			m_openTimeCount = 0.0f;
		}
		else
		{
			if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
			m_doorState = DOOR_STATE_CLOSING;
		}
	}
}

void ACardkeyDoorBody::CheckedByPlayer()
{
	if (m_doorState == DOOR_STATE_CLOSED)
	{
		if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), loading_state_color);
		m_doorState = DOOR_STATE_LOADING;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// ���m���̃A�N�^�[�����C���N�������g
		++door_eventbox_overlap_sum;

		// ���ݖh�~�p�t���O�𗧂Ă�
		is_doorbody_eventbox_overlap = true;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// ���m���̃A�N�^�[�����f�N�������g
		--door_eventbox_overlap_sum;

		// ���m���̃A�N�^�[��0�ȉ��Ȃ狲�ݖh�~�p�t���O������
		if (door_eventbox_overlap_sum <= 0)		is_doorbody_eventbox_overlap = false;
	}
}