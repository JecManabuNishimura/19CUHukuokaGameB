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
	, m_rightDoorEndPosY(0.0f)
	, m_doorFilter(0)
	, m_isSwitchOn(true)
	, m_canMove(false)
	, m_isOpened(false)
	, m_isOpening(false)
	, m_openTimeCount(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_isOverlap(false)
	, m_distanceStartToEnd(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// ���m�p�C�x���g�{�b�N�X����
	m_pEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("m_pEventTriggerBox"));
	if (m_pEventTriggerBox != NULL)
	{
		RootComponent = m_pEventTriggerBox;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// ���h�A����
	m_pLeftDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pLeftDoorComp"));
	if (m_pLeftDoorComp != NULL)
	{
		m_pLeftDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pLeftDoorComp has not been created."));

	// �E�h�A����
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
	
	// �h�A�̏����ʒu���擾
	m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	// �h�A�̎n�_����I�_�܂ł̋������v�Z
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	m_leftDoorMoveDirection = FMath::Abs(m_leftDoorEndPosY) / m_leftDoorEndPosY;
}

void AAutomaticDoorMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDoorMove(DeltaTime);

	CheckDetectSpan(DeltaTime);
}

void AAutomaticDoorMove::UpdateDoorMove(float _deltaTime)
{
	// �J�t���O�������Ă��Ȃ���Ώ������Ȃ�
	if (m_canMove == false)	return;

	if (m_isOverlap || m_isOpening)	m_openTimeCount += _deltaTime;
	else							m_openTimeCount -= _deltaTime;					

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

	// ���h�A�̏���
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	if (m_openTimeCount == m_openAndCloseTime)
	{
		m_openTimeCount = 0.0f;
	}
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
	if (m_isOpened || m_isOpening) return;

	// ���m���ɂ���Actor��Player�������烌�o�[�̏�Ԃ̃`�F�b�N���s��
	if (_otherActor->ActorHasTag("Player"))
	{
		bool isLeverFind = false;		// �Ή����郌�o�[�����݂��Ȃ����Ƀh�A���J���̂�h���p�t���O

		m_isOverlap = true;				// �v���C���[�����m���ɓ����Ă���t���O�𗧂Ă�

		for (TActorIterator<AAutomaticDoorLever> i(GetWorld()); i; ++i)
		{
			if (m_doorFilter == i->m_leverFilter)
			{
				isLeverFind = true;
				m_isSwitchOn &= i->m_isLeverOn;

				// ��ł�false���������_�ŏ����𔲂���
				if (m_isSwitchOn == false)
				{
					m_isSwitchOn = true;
					return;
				}
			}
		}
		// �h�A�ɑΉ����郌�o�[�����݂��A���̑S�Ă�ON�Ȃ�J����t���O�𗧂Ă�
		if (isLeverFind)
		{
			m_canMove = true;
			m_isOpening = true;
		}
	}
}

void AAutomaticDoorMove::OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherActor->ActorHasTag("Player"))
	{
		m_isOverlap = false;
	}
}