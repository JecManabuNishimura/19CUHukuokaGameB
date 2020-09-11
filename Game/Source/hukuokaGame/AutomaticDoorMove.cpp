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

	// 検知用イベントボックス生成
	m_pEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("m_pEventTriggerBox"));
	if (m_pEventTriggerBox != NULL)
	{
		RootComponent = m_pEventTriggerBox;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// 左ドア生成
	m_pLeftDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pLeftDoorComp"));
	if (m_pLeftDoorComp != NULL)
	{
		m_pLeftDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pLeftDoorComp has not been created."));

	// 右ドア生成
	m_pRightDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pRightDoorComp"));
	if (m_pRightDoorComp != NULL)
	{
		m_pRightDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pRightDoorComp has not been created."));

	// 関数バインド
	m_pEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorMove::OnOverlapBegin);
	m_pEventTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorMove::OnOverlapEnd);
}
AAutomaticDoorMove::~AAutomaticDoorMove()
{

}

void AAutomaticDoorMove::BeginPlay()
{
	Super::BeginPlay();
	
	// ドアの初期位置を取得
	m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	// ドアの始点から終点までの距離を計算
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
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
	// 開閉フラグが立っていなければ処理しない
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

	// 左ドアの処理
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	if (m_openTimeCount == m_openAndCloseTime)
	{
		m_openTimeCount = 0.0f;
	}
}

void AAutomaticDoorMove::CheckDetectSpan(float _deltaTime)
{
	// 開いていなければ処理をしない
	if (m_isOpened == false)	return;

	m_openTimeCount += _deltaTime;

	// 決めた時間を超えたらチェック
	if (m_openTimeCount > m_detectSpan)
	{
		// 検知できなければドアを開ける処理を降ろす
		if (m_isOverlap == false)
		{
			m_isOpened = false;
			m_canMove = true;
			// 閉じる処理をするための数値代入
			m_openTimeCount = m_openAndCloseTime;
		}
		else
		{
			// 開く処理継続のための数値代入
			m_openTimeCount = 0.0f;
		}
	}
}

void AAutomaticDoorMove::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (m_isOpened || m_isOpening) return;

	// 検知内にいるActorがPlayerだったらレバーの状態のチェックを行う
	if (_otherActor->ActorHasTag("Player"))
	{
		bool isLeverFind = false;		// 対応するレバーが存在しない時にドアが開くのを防ぐ用フラグ

		m_isOverlap = true;				// プレイヤーが検知内に入っているフラグを立てる

		for (TActorIterator<AAutomaticDoorLever> i(GetWorld()); i; ++i)
		{
			if (m_doorFilter == i->m_leverFilter)
			{
				isLeverFind = true;
				m_isSwitchOn &= i->m_isLeverOn;

				// 一つでもfalseだった時点で処理を抜ける
				if (m_isSwitchOn == false)
				{
					m_isSwitchOn = true;
					return;
				}
			}
		}
		// ドアに対応するレバーが存在し、その全てがONなら開けるフラグを立てる
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