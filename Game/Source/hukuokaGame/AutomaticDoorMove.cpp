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

	// 動的配列の初期化
	m_filterMatchLevers.Reset();

	// 検知用イベントボックス生成
	m_pEventTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("m_pEventTriggerBox"));
	if (m_pEventTriggerBox != NULL)
	{
		RootComponent = m_pEventTriggerBox;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// 左ドアのもととなるメッシュ作成
	m_pLeftDoorComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pLeftDoorComp"));
	if (m_pLeftDoorComp != NULL)
	{
		m_pLeftDoorComp->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pLeftDoorComp has not been created."));

	// 右ドアのもととなるメッシュ作成
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

	//// フィルター番号が一致するレバー取得、保存
	//TSubclassOf<AAutomaticDoorLever> findClass;
	//findClass = AAutomaticDoorLever::StaticClass();
	//TArray<AActor*> actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	//if (actors.Num() > 0)
	//{
	//	for (int idx = 0; idx < actors.Num(); ++idx)
	//	{
	//		AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(actors[idx]);
	//		// レバーのフィルター番号がドア自身のフィルター番号と一致していれば配列に追加する
	//		if (pLever->m_leverFilter == m_doorFilter)
	//		{
	//			m_filterMatchLevers.Add(pLever);
	//		}
	//	}
	//	// ドアに対応するレバーがレベルに配置されていないためエラー
	//	if (m_filterMatchLevers.Num() == 0)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("The corresponding lever is not installed on the level"));
	//	}
	//}
	//// そもそもレバーが一つもレベルに配置されていないためエラー
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("There is no lever installed on the level"));
	//}
	//
	//// ドアの初期位置を取得
	//m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	//m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	//// ドアの始点から終点までの距離を計算
	//m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	//// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
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
		// 検知フラグが立っていたら状態をOpeningへ
		if (m_isOverlap)
		{
			m_doorState = DOOR_STATE_OPENING;
			m_openTimeCount += _deltaTime;
		}
		break;

	case DOOR_STATE_OPENED:
		// 検知フラグが下がっていたら状態をClosingへ
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



	// 左ドアの処理
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_openTimeCount) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));
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
	//// 検知したActorがPlayerかEnemyだったら
	//if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	//{
	//	// 対応するレバーの状態を確認
	//	for (int idx = 0; idx < m_filterMatchLevers.Num(); ++idx)
	//	{
	//		// 一つでもレバーの状態がOFF(=false)ならreturn
	//		if (m_filterMatchLevers[idx]->m_leverFilter == false)
	//		{
	//			return;
	//		}
	//	}
	//	// 対応するレバー全ての状態がON(=true)なら検知フラグを立てる
	//	m_isOverlap = true;
	//}
}

void AAutomaticDoorMove::OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	//// 検知したActorがPlayerかEnemyだったら
	//if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	//{
	//	// 検知フラグを下げる
	//	m_isOverlap = false;
	//}
}