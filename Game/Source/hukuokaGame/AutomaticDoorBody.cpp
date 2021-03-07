//-------------------------------------------------------------------
// ファイル		：AutomaticDoorBody.cpp
// 概要			：自動ドア本体を制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/04
//-------------------------------------------------------------------

#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

AAutomaticDoorBody::AAutomaticDoorBody()
	: p_eventTriggerBox_(NULL)
	, p_left_door_comp_(NULL)
	, p_right_door_comp_(NULL)
	, p_lamp_for_decide_pos_(NULL)
	, p_lever_state_lamp_(NULL)
	, p_lever_on_state_material_(NULL)
	, p_lever_off_state_material_(NULL)
	, p_sound_door_open_(NULL)
	, p_sound_door_close_(NULL)
	, detect_span_(3.0f)
	, open_and_close_time_(1.0f)
	, left_door_start_posY_(0.0f)
	, left_door_end_posY_(0.0f)
	, right_door_start_posY_(0.0f)
	, door_filter_num_(0)
	, lamp_generate_pos_(FVector::ZeroVector)
	, is_switch_on_(false)
	, is_overlap_(false)
	, open_time_count_(0.0f)
	, required_time_for_open_and_close_(0.0f)
	, left_door_dire_for_move_(1.0f)
	, distance_start_to_end_(0.0f)
	, detect_num_(0)
	, door_state_(kDoorStateClosed)
	, p_alert_lever_(NULL)
	, is_alert_lever_on_(false)
	, items_Mission_Num(0)
	, next_Items_Mission_Num(0)
	, isMissionComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// 動的配列の初期化
	filter_match_levers_.Reset();
	match_lever_state_lamps_.Reset();

	// 検知用イベントボックス生成
	p_eventTriggerBox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("p_eventTriggerBox_"));
	if (p_eventTriggerBox_ != NULL)
	{
		RootComponent = p_eventTriggerBox_;
	}
	else	UE_LOG(LogTemp, Error, TEXT("m_pEventTrigerBox has not been created."));

	// 左ドアのもととなるメッシュ作成
	p_left_door_comp_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_left_door_comp_"));
	if (p_left_door_comp_ != NULL)
	{
		p_left_door_comp_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_left_door_comp_ has not been created."));

	// 右ドアのもととなるメッシュ作成
	p_right_door_comp_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_right_door_comp_"));
	if (p_right_door_comp_ != NULL)
	{
		p_right_door_comp_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_right_door_comp_ has not been created."));

	p_lamp_for_decide_pos_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("p_lamp_for_decide_pos_"));
	if (p_lamp_for_decide_pos_ != NULL)
	{
		p_lamp_for_decide_pos_->SetupAttachment(RootComponent);
	}
	else	UE_LOG(LogTemp, Error, TEXT("p_lamp_for_decide_pos_ has not been created."));

	// 関数バインド
	p_eventTriggerBox_->OnComponentBeginOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapBegin);
	p_eventTriggerBox_->OnComponentEndOverlap.AddDynamic(this, &AAutomaticDoorBody::OnOverlapEnd);
}
AAutomaticDoorBody::~AAutomaticDoorBody()
{
}

void AAutomaticDoorBody::BeginPlay()
{
	Super::BeginPlay();

	// フィルター番号が一致するレバーと非常レバー取得、保存
	TSubclassOf<AAutomaticDoorLever> findClass;
	findClass = AAutomaticDoorLever::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorLever* p_lever = Cast<AAutomaticDoorLever>(actors[idx]);

			// レバーのフィルター番号がドア自身のフィルター番号と一致していれば配列に追加する
			if (p_lever->GetLeverFilter() == door_filter_num_)
			{
				filter_match_levers_.Add(p_lever);
			}

			// 非常レバーなら保存
			if (p_lever->ActorHasTag("AlertLever"))
			{
				p_alert_lever_ = p_lever;
			}
		}

		filter_match_levers_num_ = filter_match_levers_.Num();

		// ドアに対応するレバーがレベルに配置されていればその数ランプを生成
		if (filter_match_levers_num_ != 0)
		{
			lamp_generate_pos_ = p_lamp_for_decide_pos_->GetRelativeLocation();

			FString lamp_name = "lever_state_lamp";

			if (p_lever_state_lamp_ != NULL)
			{
				for (int i = 0; i < filter_match_levers_num_; ++i)
				{
					match_lever_state_lamps_.Add(NewObject<UStaticMeshComponent>(this));
					match_lever_state_lamps_[i]->RegisterComponent();
					match_lever_state_lamps_[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
					match_lever_state_lamps_[i]->SetStaticMesh(p_lever_state_lamp_);
					match_lever_state_lamps_[i]->SetRelativeLocation(FVector(lamp_generate_pos_.X, ((((40.f * (i + 1)) + (-20.f * (float)(filter_match_levers_num_ + 1))) * p_lamp_for_decide_pos_->GetRelativeScale3D().X) + lamp_generate_pos_.Y), lamp_generate_pos_.Z));
					match_lever_state_lamps_[i]->SetRelativeScale3D(p_lamp_for_decide_pos_->GetRelativeScale3D());
				}
			}
		}
	}

	p_lamp_for_decide_pos_->DestroyComponent();

	// ドアの初期位置を取得
	left_door_start_posY_ = p_left_door_comp_->GetRelativeLocation().Y;
	right_door_start_posY_ = p_right_door_comp_->GetRelativeLocation().Y;

	// ドアの始点から終点までの距離を計算
	distance_start_to_end_ = FMath::Abs(left_door_end_posY_ - left_door_start_posY_);

	// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
	left_door_dire_for_move_ = FMath::Abs(left_door_end_posY_) / left_door_end_posY_;
}

void AAutomaticDoorBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAlertLeverState();

	UpdateDoorState();

	UpdateDoorMove(DeltaTime);
}

void AAutomaticDoorBody::CheckAlertLeverState()
{
	if (is_alert_lever_on_)	return;

	if (p_alert_lever_ != NULL)
	{
		is_alert_lever_on_ = p_alert_lever_->GetLeverState();

		if (is_alert_lever_on_)
		{
			is_switch_on_ = true;
			door_state_ = kDoorStateOpening;
		}
	}
}

void AAutomaticDoorBody::UpdateDoorState()
{
	switch (door_state_)
	{
	case kDoorStateClosed:
		if (is_switch_on_ && is_overlap_)
		{
			door_state_ = kDoorStateOpening;

			// ドアが開く時のSEを鳴らす
			if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
		}
		break;

	case kDoorStateOpened:
		if (!is_switch_on_ && !is_alert_lever_on_)
		{
			door_state_ = kDoorStateClosing;
		}
		break;

	case kDoorStateClosing:
		if (is_switch_on_ && is_overlap_)
		{
			door_state_ = kDoorStateOpening;

			// ドアが開く時のSEを鳴らす
			if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
		}
		else if (required_time_for_open_and_close_ <= 0.0f)
		{
			door_state_ = kDoorStateClosed;
		}
		break;

	case kDoorStateOpening:
		if (!is_switch_on_ && !is_alert_lever_on_)
		{
			door_state_ = kDoorStateClosing;
		}
		else if (required_time_for_open_and_close_ >= open_and_close_time_)
		{
			door_state_ = kDoorStateOpened;
		}
		break;

	default:
		break;
	}
}

void AAutomaticDoorBody::UpdateDoorMove(float _deltaTime)
{
	switch (door_state_)
	{
	case kDoorStateOpened:
		CheckDetectSpan(_deltaTime);
	case kDoorStateClosed:
		return;
		break;

	case kDoorStateClosing:
		open_time_count_ = 0.0f;
		required_time_for_open_and_close_ -= _deltaTime;		
		if (required_time_for_open_and_close_ < 0.0f) { required_time_for_open_and_close_ = 0.0f; }
		break;

	case kDoorStateOpening:
		required_time_for_open_and_close_ += _deltaTime;
		if (required_time_for_open_and_close_ > open_and_close_time_) { required_time_for_open_and_close_ = open_and_close_time_; }
		break;

	default:
		break;
	}

	// 左ドアの処理
	float newLocationY = left_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_);
	p_left_door_comp_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = right_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_ * -1.0f);
	p_right_door_comp_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void AAutomaticDoorBody::CheckDetectSpan(float _deltaTime)
{
	if (is_alert_lever_on_)	return;

	open_time_count_ += _deltaTime;

	// 決めた時間を超えたらチェック
	if (open_time_count_ > detect_span_)
	{
		// 検知できなければ状態をCLOSINGへ
		if (!is_overlap_)
		{
			door_state_ = kDoorStateClosing;

			// ドアが閉じる時のSEを鳴らす
			if (p_sound_door_close_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_close_, GetActorLocation());
		}
		else
		{
			// 開く処理継続のための数値代入
			open_time_count_ = 0.0f;
		}
	}
}

void AAutomaticDoorBody::OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	// 検知したActorがPlayerかEnemyだったら
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 範囲内検知数をインクリメント
		++detect_num_;

		// 検知フラグを立てる
		is_overlap_ = true;

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
		--detect_num_;

		// 範囲内に誰もいないなら検知フラグを下げる
		if (detect_num_ == 0)
		{
			is_overlap_ = false;
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
					match_lever_state_lamps_[i]->SetMaterial(0, p_lever_off_state_material_);
					is_switch_on_ = false;
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
			match_lever_state_lamps_[i]->SetMaterial(0, p_lever_on_state_material_);
			switch_state = switch_state && true;
		}
		else
		{
			match_lever_state_lamps_[i]->SetMaterial(0, p_lever_off_state_material_);
			switch_state = switch_state && false;
		}
	}
	
	// 対応するレバー全ての状態がON(=true)なら検知フラグを立てる
	is_switch_on_ = switch_state;

	// ミッションに反映する場合、スマホのミッションをアップデート (作成者:林雲暉)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			if (is_switch_on_ == true) {
				GoToPlayerCharacterAndUpdateMission();
			} // end if()
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
		isMissionComplete = true;
		bool tempbool = false;
		player1->UpdateTheMission(0, this->next_Items_Mission_Num, tempbool);
	} // end if()
	
} // GoToPlayerCharacterAndUpdateMission()
