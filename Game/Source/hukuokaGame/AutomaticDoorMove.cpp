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
	, m_detectSpan(3.0f)
	, m_doorStartPosY(0.0f)
	, m_doorEndPosY(0.0f)
	, m_isLeverOn(true)
	, m_openTimeCount(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	m_pEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("m_pEventTriggerBox"));
	RootComponent = m_pEventTriggerBox;

	// �֐��o�C���h
	m_pEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorMove::OnOverlapBegin);
}
AAutomaticDoorMove::~AAutomaticDoorMove()
{

}

// Called when the game starts or when spawned
void AAutomaticDoorMove::BeginPlay()
{
	Super::BeginPlay();
	

}

void AAutomaticDoorMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AAutomaticDoorMove::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor->ActorHasTag("Player"))
	{
		// �h�A���J������
	}
}

