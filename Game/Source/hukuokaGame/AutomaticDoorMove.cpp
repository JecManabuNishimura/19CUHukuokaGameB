//-------------------------------------------------------------------
// ファイル		：AutomaticDoorMove.cpp
// 概要			：自動ドアを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/20
//-------------------------------------------------------------------

#include "AutomaticDoorMove.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

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

	// 関数バインド
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
		// ドアを開く処理
	}
}

