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

	// 動的配列の初期化
	filter_match_levers_.Reset();
	match_lever_state_lamps_.Reset();

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

	lamp_for_decide_pos_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("lamp_for_decide_pos_"));
	if (lamp_for_decide_pos_ != NULL)
	{
		lamp_for_decide_pos_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("lamp_for_decide_pos_ has not been created."));

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
				filter_match_levers_.Add(pLever);
			}
		}

		filter_match_levers_num_ = filter_match_levers_.Num();

		// ドアに対応するレバーがレベルに配置されていればその数ランプを生成
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
		// ドアに対応するレバーがレベルに配置されていない旨をログ出力
		else		
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding lever is not installed on the level !"));
		}
	}
	// そもそもレバーが一つもレベルに配置されていないためエラー
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no lever installed on the level !"));
	}

	lamp_for_decide_pos_->DestroyComponent();

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
void AAutomaticDoorBody::UpdateSwitchState(const AAutomaticDoorLever* const operated_lever)
{
	// レバー側から呼び出されなおかつその状態がOFFだった場合
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

	// ドア本体から呼び出された場合および操作されたレバーがONだった場合
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

	// 対応するレバー全ての状態がON(=true)なら検知フラグを立てる
	m_isSwitchOn = switch_state;

	// ミッションに反映する場合、スマホのミッションをアップデート (作成者:林雲暉)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			if (m_isSwitchOn == true)
				GoToPlayerCharacterAndUpdateMission();

		} // end if()
	} // end if()
}

// プレイヤーのUpdateTheMissionを呼び出すためのメゾット (作成者:林雲暉)
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
