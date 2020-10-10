//-------------------------------------------------------------------
// ファイル		：AutomaticDoorBody.cpp
// 概要			：自動ドア本体を制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/04
//-------------------------------------------------------------------

#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

AAutomaticDoorBody::AAutomaticDoorBody()
	: m_pEventTriggerBox(NULL)
	, m_pLeftDoorComp(NULL)
	, m_pRightDoorComp(NULL)
	, m_detectSpan(3.0f)
	, m_openAndCloseTime(1.0f)
	, m_leftDoorStartPosY(0.0f)
	, m_leftDoorEndPosY(0.0f)
	, m_rightDoorStartPosY(0.0f)
	, m_doorFilter(0)
	, m_isSwitchOn(false)
	, m_isOverlap(false)
	, m_openTimeCount(0.0f)
	, m_requiredTime(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_distanceStartToEnd(0.0f)
	, m_detectNum(0)
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
	m_pEventTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapBegin);
	m_pEventTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapEnd);
}
AAutomaticDoorBody::~AAutomaticDoorBody()
{

}

void AAutomaticDoorBody::BeginPlay()
{
	Super::BeginPlay();

	// フィルター番号が一致するレバー取得、保存
	TSubclassOf<AAutomaticDoorLever> findClass;
	findClass = AAutomaticDoorLever::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(actors[idx]);
			// レバーのフィルター番号がドア自身のフィルター番号と一致していれば配列に追加する
			if (pLever->GetLeverFilter() == m_doorFilter)
			{
				m_filterMatchLevers.Add(pLever);
			}
		}
		// ドアに対応するレバーがレベルに配置されていないためエラー
		if (m_filterMatchLevers.Num() == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding lever is not installed on the level !"));
		}
	}
	// そもそもレバーが一つもレベルに配置されていないためエラー
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no lever installed on the level !"));
	}

	// ドアの初期位置を取得
	m_leftDoorStartPosY = m_pLeftDoorComp->GetRelativeLocation().Y;
	m_rightDoorStartPosY = m_pRightDoorComp->GetRelativeLocation().Y;

	// ドアの始点から終点までの距離を計算
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
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

	// 左ドアの処理
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection);
	m_pLeftDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection * -1.0f);
	m_pRightDoorComp->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void AAutomaticDoorBody::CheckDetectSpan(float _deltaTime)
{
	m_openTimeCount += _deltaTime;

	UE_LOG(LogTemp, Error, TEXT("m_openTimeCount : %f"), m_openTimeCount);

	// 決めた時間を超えたらチェック
	if (m_openTimeCount > m_detectSpan)
	{
		// 検知できなければ状態をCLOSINGへ
		if (!m_isOverlap)
		{
			m_doorState = DOOR_STATE_CLOSING;
		}
		else
		{
			// 開く処理継続のための数値代入
			m_openTimeCount = 0.0f;
		}
	}
}

void AAutomaticDoorBody::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	// 検知したActorがPlayerかEnemyだったら
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 範囲内検知数をインクリメント
		++m_detectNum;

		// 検知フラグを立てる
		m_isOverlap = true;

		// ドアの作動フラグの更新
		UpdateSwitchState();
	}
}

void AAutomaticDoorBody::OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	// 検知したActorがPlayerかEnemyだったら
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 範囲内検知数をデクリメント
		--m_detectNum;

		// 範囲内に誰もいないなら検知フラグを下げる
		if (m_detectNum == 0)
		{
			m_isOverlap = false;
		}
	}
}

// ドア本体のスイッチの更新
void AAutomaticDoorBody::UpdateSwitchState(const bool _isLeverOn)
{
	// レバー側から呼び出されてfalseが渡されたら作動フラグを下げる
	if (!_isLeverOn)
	{ 
		m_isSwitchOn = false;
		return; 
	}

	// 対応するレバーの状態を確認
	for (int idx = 0; idx < m_filterMatchLevers.Num(); ++idx)
	{
		if (m_filterMatchLevers[idx] == NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding lever is not installed on the level !"));
			return;
		}
			// 一つでもレバーの状態がOFF(=false)ならreturn
		if (m_filterMatchLevers[idx]->GetLeverState() == false)
		{
			return;
		}
	}
	// 対応するレバー全ての状態がON(=true)なら検知フラグを立てる
	m_isSwitchOn = true;
}