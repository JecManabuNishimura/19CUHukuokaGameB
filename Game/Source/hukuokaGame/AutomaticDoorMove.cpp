//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorMove.cpp
// �T�v			�F�����h�A�𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/20
//-------------------------------------------------------------------

#include "AutomaticDoorMove.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

AAutomaticDoorMove::AAutomaticDoorMove()
	: m_pEventTriggerBox(NULL)
	, m_pLeftDoorComp(NULL)
	, m_pRightDoorComp(NULL)
	, m_detectSpan(3.0f)
	, m_openAndCloseTime(1.0f)
	, m_leftDoorStartPosY(0.0f)
	, m_leftDoorEndPosY(0.0f)
	, m_rightDoorStartPosY(0.0f)
	, m_doorFilter(0)
	, m_isSwitchOn(true)
	, m_canMove(false)
	, m_isOpened(false)
	, m_isOpening(false)
	, m_isOverlap(false)
	, m_openTimeCount(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_distanceStartToEnd(0.0f)
	, m_doorState(DOOR_STATE_CLOSED)
{
	PrimaryActorTick.bCanEverTick = true;

	// ���I�z��̏�����
	m_filterMatchLevers.Reset();

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

	// �֐��o�C���h
	m_pEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorMove::OnOverlapBegin);
	m_pEventTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorMove::OnOverlapEnd);
}
AAutomaticDoorMove::~AAutomaticDoorMove()
{

}

void AAutomaticDoorMove::BeginPlay()
{
	Super::BeginPlay();

	//// �t�B���^�[�ԍ�����v���郌�o�[�擾�A�ۑ�
	//TSubclassOf<AAutomaticDoorLever> findClass;
	//findClass = AAutomaticDoorLever::StaticClass();
	//TArray<AActor*> actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	//if (actors.Num() > 0)
	//{
	//	for (int idx = 0; idx < actors.Num(); ++idx)
	//	{
	//		AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(actors[idx]);
	//		// ���o�[�̃t�B���^�[�ԍ����h�A���g�̃t�B���^�[�ԍ��ƈ�v���Ă���Δz��ɒǉ�����
	//		if (pLever->m_leverFilter == m_doorFilter)
	//		{
	//			m_filterMatchLevers.Add(pLever);
	//		}
	//	}
	//	// �h�A�ɑΉ����郌�o�[�����x���ɔz�u����Ă��Ȃ����߃G���[
	//	if (m_filterMatchLevers.Num() == 0)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("The corresponding lever is not installed on the level"));
	//	}
	//}
	//// �����������o�[��������x���ɔz�u����Ă��Ȃ����߃G���[
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("There is no lever installed on the level"));
	//}
	//
	//// �h�A�̏����ʒu���擾
	//m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	//m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	//// �h�A�̎n�_����I�_�܂ł̋������v�Z
	//m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	//// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	//m_leftDoorMoveDirection = FMath::Abs(m_leftDoorEndPosY) / m_leftDoorEndPosY;
}

void AAutomaticDoorMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAutomaticDoorMove::UpdateDoorMove(float _deltaTime)
{
	switch (m_doorState)
	{
	case DOOR_STATE_CLOSED:
		// ���m�t���O�������Ă������Ԃ�Opening��
		if (m_isOverlap)
		{
			m_doorState = DOOR_STATE_OPENING;
			m_openTimeCount += _deltaTime;
		}
		break;

	case DOOR_STATE_OPENED:
		// ���m�t���O���������Ă������Ԃ�Closing��
		if (!m_isOverlap)
		{
			m_doorState = DOOR_STATE_CLOSING;
		}
		break;

	case DOOR_STATE_CLOSING:
		m_openTimeCount -= _deltaTime;
		break;

	case DOOR_STATE_OPENING:
		m_openTimeCount += _deltaTime;
		break;

	default:
		break;
	}

	if (m_openTimeCount > m_openAndCloseTime)
	{
		m_openTimeCount = m_openAndCloseTime;
		m_isOpened = true;
		m_canMove = false;
		m_isOpening = false;
	}
	else if (m_openTimeCount < 0.0f)
	{
		m_openTimeCount = 0.0f;
		m_canMove = false;
	}

	if (m_openTimeCount == m_openAndCloseTime)
	{
		m_openTimeCount = 0.0f;
	}



	// ���h�A�̏���
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void AAutomaticDoorMove::CheckDetectSpan(float _deltaTime)
{
	// �J���Ă��Ȃ���Ώ��������Ȃ�
	if (m_isOpened == false)	return;

	m_openTimeCount += _deltaTime;

	// ���߂����Ԃ𒴂�����`�F�b�N
	if (m_openTimeCount > m_detectSpan)
	{
		// ���m�ł��Ȃ���΃h�A���J���鏈�����~�낷
		if (m_isOverlap == false)
		{
			m_isOpened = false;
			m_canMove = true;
			// ���鏈�������邽�߂̐��l���
			m_openTimeCount = m_openAndCloseTime;
		}
		else
		{
			// �J�������p���̂��߂̐��l���
			m_openTimeCount = 0.0f;
		}
	}
}

void AAutomaticDoorMove::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	//// ���m����Actor��Player��Enemy��������
	//if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	//{
	//	// �Ή����郌�o�[�̏�Ԃ��m�F
	//	for (int idx = 0; idx < m_filterMatchLevers.Num(); ++idx)
	//	{
	//		// ��ł����o�[�̏�Ԃ�OFF(=false)�Ȃ�return
	//		if (m_filterMatchLevers[idx]->m_leverFilter == false)
	//		{
	//			return;
	//		}
	//	}
	//	// �Ή����郌�o�[�S�Ă̏�Ԃ�ON(=true)�Ȃ猟�m�t���O�𗧂Ă�
	//	m_isOverlap = true;
	//}
}

void AAutomaticDoorMove::OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	//// ���m����Actor��Player��Enemy��������
	//if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	//{
	//	// ���m�t���O��������
	//	m_isOverlap = false;
	//}
}