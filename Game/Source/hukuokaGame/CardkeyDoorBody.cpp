//-------------------------------------------------------------------
// ファイル		：CardkeyDoorBody.cpp
// 概要			：カードキー対応のドアを制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/22
//-------------------------------------------------------------------

#include "CardkeyDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

ACardkeyDoorBody::ACardkeyDoorBody()
	: p_door_body_eventtriggerbox_(NULL)
	, p_cardreader_mesh_1_(NULL)
	, p_cardreader_mesh_2_(NULL)
	, p_leftdoor_mesh_(NULL)
	, p_rightdoor_mesh_(NULL)
	, p_door_state_mesh_1_(NULL)
	, p_door_state_mesh_2_(NULL)
	, p_door_state_material_(NULL)
	, p_material_instance_dynamic_(NULL)
	, p_sound_door_open_(NULL)
	, p_sound_door_close_(NULL)
	, p_sound_loading_success_(NULL)
	, p_sound_loading_error_(NULL)
	, p_player_character_(NULL)
	, detect_span_(3.0f)
	, open_and_close_time_(1.0f)
	, check_cardkey_time_(2.0f)
	, display_error_time_(3.0f)
	, count_for_check_display_time_(0.0f)
	, left_door_start_posY_(0.0f)
	, left_door_end_posY_(0.0f)
	, right_door_start_posY_(0.0f)
	, door_filter_num_(0)
	, door_state_(kDoorStateClosed)
	, is_doorbody_eventbox_overlap_(false)
	, door_eventbox_overlap_sum_(0)
	, open_time_count_(0.0f)
	, required_time_for_open_and_close_(0.0f)
	, left_door_dire_for_move_(1.0f)
	, distance_start_to_end_(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Root用SceneComponent生成
	USceneComponent* scene_comp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// ドア本体の検知トリガーボックス生成
	p_door_body_eventtriggerbox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("p_door_body_eventtriggerbox_"));

	// カードリーダーのメッシュその1作成
	p_cardreader_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_1"));

	// カードリーダーのメッシュその2作成
	p_cardreader_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_2"));

	// 左ドアのメッシュ作成
	p_leftdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));

	// 右ドアのメッシュ作成
	p_rightdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_Parent"));

	// 左ドアのロック状態メッシュ作成
	p_door_state_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorStateMesh"));

	// 右ドアのロック状態メッシュ作成
	p_door_state_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorStateMesh"));

	p_door_state_material_ = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/Materials/CardReaderStateMaterial")));
	
	if (scene_comp != NULL)				RootComponent = scene_comp;
	if (p_leftdoor_mesh_ != NULL)			p_leftdoor_mesh_->SetupAttachment(RootComponent);
	if (p_rightdoor_mesh_ != NULL)		p_rightdoor_mesh_->SetupAttachment(RootComponent);

	if (p_cardreader_mesh_1_ != NULL)		p_cardreader_mesh_1_->SetupAttachment(RootComponent);

	if (p_cardreader_mesh_2_ != NULL)		p_cardreader_mesh_2_->SetupAttachment(RootComponent);

	if (p_door_state_mesh_1_ != NULL)		p_door_state_mesh_1_->SetupAttachment(p_cardreader_mesh_1_);

	if (p_door_state_mesh_2_ != NULL)		p_door_state_mesh_2_->SetupAttachment(p_cardreader_mesh_2_);

	if (p_door_body_eventtriggerbox_ != NULL)
	{
		p_door_body_eventtriggerbox_->SetupAttachment(RootComponent);

		p_door_body_eventtriggerbox_->OnComponentBeginOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapBegin);
		p_door_body_eventtriggerbox_->OnComponentEndOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapEnd);
	}
}
ACardkeyDoorBody::~ACardkeyDoorBody()
{
}

void ACardkeyDoorBody::BeginPlay()
{
	Super::BeginPlay();

	if (p_door_state_material_ != NULL)
	{
		p_material_instance_dynamic_ = p_door_state_mesh_1_->CreateAndSetMaterialInstanceDynamicFromMaterial(0, p_door_state_material_);
		if (p_material_instance_dynamic_ != NULL)
		{
			p_door_state_mesh_1_->SetMaterial(0, p_material_instance_dynamic_);
			p_door_state_mesh_2_->SetMaterial(0, p_material_instance_dynamic_);
			p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
		}
		else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("p_material_instance_dynamic_ is NULL."));
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("p_door_state_material_ is NULL."));	

	// ドアの初期位置を取得
	left_door_start_posY_ = p_leftdoor_mesh_->GetRelativeLocation().Y;
	right_door_start_posY_ = p_rightdoor_mesh_->GetRelativeLocation().Y;

	// ドアの始点から終点までの距離を計算
	distance_start_to_end_ = FMath::Abs(left_door_end_posY_ - left_door_start_posY_);

	// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
	left_door_dire_for_move_ = FMath::Abs(left_door_end_posY_) / left_door_end_posY_;

	p_player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (p_player_character_ == NULL)
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
	switch (door_state_)
	{
	case kDoorStateLoading:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= check_cardkey_time_)
		{			
			count_for_check_display_time_ = 0.0f;
			if ((p_player_character_->GetHaveCardkeyState() & (1 << door_filter_num_)) != 0)
			{
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_success_);

				if (p_sound_loading_success_ != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_loading_success_, GetActorLocation());

					// ドアが開く時のSEを鳴らす
					if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
				}
				door_state_ = kDoorStateOpening;
			}
			else
			{
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_error_);

				if (p_sound_loading_error_ != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_loading_error_, GetActorLocation());
				}
				door_state_ = kDoorStateError;
			}
		}
		break;

	case kDoorStateError:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= display_error_time_)
		{
			count_for_check_display_time_ = 0.0f;
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
				door_state_ = kDoorStateClosed;
			
		}
		break;

	case kDoorStateOpened:
		CheckDetectSpan(_deltatime);
		break;

	case kDoorStateClosed:
		break;

	case kDoorStateClosing:
		if (is_doorbody_eventbox_overlap_)
		{
			door_state_ = kDoorStateOpening;
		}

		if (required_time_for_open_and_close_ <= 0.0f)
		{
			door_state_ = kDoorStateClosed;
		}
		break;

	case kDoorStateOpening:
		if (required_time_for_open_and_close_ >= open_and_close_time_)
		{
			door_state_ = kDoorStateOpened;
		}
		break;

	default:
		break;
	}
}

void ACardkeyDoorBody::UpdateDoorMove(float _deltatime)
{
	switch (door_state_)
	{
	case kDoorStateOpened:
	case kDoorStateLoading:
	case kDoorStateError:
	case kDoorStateClosed:
		return;
		break;

	case kDoorStateClosing:
		open_time_count_ = 0.0f;
		required_time_for_open_and_close_ -= _deltatime;
		if (required_time_for_open_and_close_ < 0.0f) { required_time_for_open_and_close_ = 0.0f; }
		break;

	case kDoorStateOpening:
		required_time_for_open_and_close_ += _deltatime;
		if (required_time_for_open_and_close_ > open_and_close_time_) { required_time_for_open_and_close_ = open_and_close_time_; }
		break;

	default:
		return;
		break;
	}

	// 左ドアの処理
	float newLocationY = left_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_);
	p_leftdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = right_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_ * -1.0f);
	p_rightdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void ACardkeyDoorBody::CheckDetectSpan(float _deltatime)
{
	open_time_count_ += _deltatime;

	// 決めた時間を超えたらチェック
	if (open_time_count_ > detect_span_)
	{
		// 検知できなければ状態をCLOSINGへ
		if (is_doorbody_eventbox_overlap_)
		{
			// 開く処理継続のための数値代入
			open_time_count_ = 0.0f;
		}
		else
		{
			if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
			door_state_ = kDoorStateClosing;

			// ドアが閉じる時のSEを鳴らす
			if (p_sound_door_close_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_close_, GetActorLocation());
		}
	}
}

void ACardkeyDoorBody::CheckedByPlayer()
{
	if (door_state_ == kDoorStateClosed)
	{
		// カード読み込み音を鳴らす
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_when_checked_, GetActorLocation());
		
		if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_loading_);
		door_state_ = kDoorStateLoading;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 検知内のアクター数をインクリメント
		++door_eventbox_overlap_sum_;

		// 挟み防止用フラグを立てる
		is_doorbody_eventbox_overlap_ = true;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 検知内のアクター数をデクリメント
		--door_eventbox_overlap_sum_;

		// 検知内のアクターが0以下なら挟み防止用フラグを下す
		if (door_eventbox_overlap_sum_ <= 0)		is_doorbody_eventbox_overlap_ = false;
	}
}