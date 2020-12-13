//-------------------------------------------------------------------
// ファイル		：CardkeyDoorBody.cpp
// 概要			：カードキー対応のドアを制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/22
//-------------------------------------------------------------------

#include "CardkeyDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

ACardkeyDoorBody::ACardkeyDoorBody()
	: door_body_eventtriggerbox_(NULL)
	, cardreader_mesh_1_(NULL)
	, cardreader_mesh_2_(NULL)
	, leftdoor_mesh_(NULL)
	, rightdoor_mesh_(NULL)
	, door_state_mesh_1_(NULL)
	, door_state_mesh_2_(NULL)
	, door_state_material(NULL)
	, material_instance_dynamic_(NULL)
	, sound_loading_success(NULL)
	, sound_loading_error(NULL)
	, player_character_(NULL)
	, m_detectSpan(3.0f)
	, m_openAndCloseTime(1.0f)
	, check_cardkey_time_(2.0f)
	, display_error_time_(3.0f)
	, count_for_check_display_time_(0.0f)
	, m_leftDoorStartPosY(0.0f)
	, m_leftDoorEndPosY(0.0f)
	, m_rightDoorStartPosY(0.0f)
	, m_doorFilter(0)
	, m_doorState(DOOR_STATE_CLOSED)
	, is_doorbody_eventbox_overlap(false)
	, door_eventbox_overlap_sum(0)
	, m_openTimeCount(0.0f)
	, m_requiredTime(0.0f)
	, m_leftDoorMoveDirection(1.0f)
	, m_distanceStartToEnd(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Root用SceneComponent生成
	USceneComponent* scene_comp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// ドア本体の検知トリガーボックス生成
	door_body_eventtriggerbox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("door_body_eventtriggerbox_"));

	// カードリーダーのメッシュその1作成
	cardreader_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_1"));

	// カードリーダーのメッシュその2作成
	cardreader_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_2"));

	// 左ドアのメッシュ作成
	leftdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));

	// 右ドアのメッシュ作成
	rightdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_Parent"));

	// 左ドアのロック状態メッシュ作成
	door_state_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorStateMesh"));

	// 右ドアのロック状態メッシュ作成
	door_state_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorStateMesh"));

	door_state_material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/Materials/CardReaderStateMaterial")));
	
	if (scene_comp != NULL)				RootComponent = scene_comp;
	if (leftdoor_mesh_ != NULL)			leftdoor_mesh_->SetupAttachment(RootComponent);
	if (rightdoor_mesh_ != NULL)		rightdoor_mesh_->SetupAttachment(RootComponent);

	if (cardreader_mesh_1_ != NULL)		cardreader_mesh_1_->SetupAttachment(RootComponent);

	if (cardreader_mesh_2_ != NULL)		cardreader_mesh_2_->SetupAttachment(RootComponent);

	if (door_state_mesh_1_ != NULL)		door_state_mesh_1_->SetupAttachment(cardreader_mesh_1_);

	if (door_state_mesh_2_ != NULL)		door_state_mesh_2_->SetupAttachment(cardreader_mesh_2_);

	if (door_body_eventtriggerbox_ != NULL)
	{
		door_body_eventtriggerbox_->SetupAttachment(RootComponent);

		door_body_eventtriggerbox_->OnComponentBeginOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapBegin);
		door_body_eventtriggerbox_->OnComponentEndOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapEnd);
	}
}
ACardkeyDoorBody::~ACardkeyDoorBody()
{
}

void ACardkeyDoorBody::BeginPlay()
{
	Super::BeginPlay();

	if (door_state_material != NULL)
	{
		material_instance_dynamic_ = door_state_mesh_1_->CreateAndSetMaterialInstanceDynamicFromMaterial(0, door_state_material);
		if (material_instance_dynamic_ != NULL)
		{
			door_state_mesh_1_->SetMaterial(0, material_instance_dynamic_);
			door_state_mesh_2_->SetMaterial(0, material_instance_dynamic_);
			material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
		}
		else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("material_instance_dynamic_ is NULL."));
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Door_state_material is NULL."));	

	// ドアの初期位置を取得
	m_leftDoorStartPosY = leftdoor_mesh_->GetRelativeLocation().Y;
	m_rightDoorStartPosY = rightdoor_mesh_->GetRelativeLocation().Y;

	// ドアの始点から終点までの距離を計算
	m_distanceStartToEnd = FMath::Abs(m_leftDoorEndPosY - m_leftDoorStartPosY);

	// 左ドアの開くときの移動方向を設定(右ドアはこれを反転して利用)
	m_leftDoorMoveDirection = FMath::Abs(m_leftDoorEndPosY) / m_leftDoorEndPosY;

	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player_character_ == NULL)
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
	switch (m_doorState)
	{
	case DOOR_STATE_LOADING:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= check_cardkey_time_)
		{			
			count_for_check_display_time_ = 0.0f;
			if ((player_character_->player_state & (1 << m_doorFilter)) != 0)
			{
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), success_state_color);

				if (sound_loading_success != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_loading_success, GetActorLocation(), 1.0f, 1.0f, 0.0f);
				}
				m_doorState = DOOR_STATE_OPENING;
			}
			else
			{
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), error_state_color);

				if (sound_loading_error != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_loading_error, GetActorLocation(), 1.0f, 1.0f, 0.0f);
				}
				m_doorState = DOOR_STATE_ERROR;
			}
		}
		break;

	case DOOR_STATE_ERROR:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= display_error_time_)
		{
			count_for_check_display_time_ = 0.0f;
				if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
				m_doorState = DOOR_STATE_CLOSED;
			
		}
		break;

	case DOOR_STATE_OPENED:
		CheckDetectSpan(_deltatime);
		break;

	case DOOR_STATE_CLOSED:
		break;

	case DOOR_STATE_CLOSING:
		if (is_doorbody_eventbox_overlap)
		{
			m_doorState = DOOR_STATE_OPENING;
		}

		if (m_requiredTime <= 0.0f)
		{
			m_doorState = DOOR_STATE_CLOSED;
		}
		break;

	case DOOR_STATE_OPENING:
		if (m_requiredTime >= m_openAndCloseTime)
		{
			m_doorState = DOOR_STATE_OPENED;
		}
		break;

	default:
		break;
	}
}

void ACardkeyDoorBody::UpdateDoorMove(float _deltatime)
{
	switch (m_doorState)
	{
	case DOOR_STATE_OPENED:
	case DOOR_STATE_LOADING:
	case DOOR_STATE_ERROR:
	case DOOR_STATE_CLOSED:
		return;
		break;

	case DOOR_STATE_CLOSING:
		m_openTimeCount = 0.0f;
		m_requiredTime -= _deltatime;
		if (m_requiredTime < 0.0f) { m_requiredTime = 0.0f; }
		break;

	case DOOR_STATE_OPENING:
		m_requiredTime += _deltatime;
		if (m_requiredTime > m_openAndCloseTime) { m_requiredTime = m_openAndCloseTime; }
		break;

	default:
		return;
		break;
	}

	// 左ドアの処理
	float newLocationY = m_leftDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection);
	leftdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// 右ドアの処理
	newLocationY = m_rightDoorStartPosY + ((m_distanceStartToEnd * m_requiredTime) * m_leftDoorMoveDirection * -1.0f);
	rightdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void ACardkeyDoorBody::CheckDetectSpan(float _deltatime)
{
	m_openTimeCount += _deltatime;

	// 決めた時間を超えたらチェック
	if (m_openTimeCount > m_detectSpan)
	{
		// 検知できなければ状態をCLOSINGへ
		if (is_doorbody_eventbox_overlap)
		{
			// 開く処理継続のための数値代入
			m_openTimeCount = 0.0f;
		}
		else
		{
			if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), standby_state_color);
			m_doorState = DOOR_STATE_CLOSING;
		}
	}
}

void ACardkeyDoorBody::CheckedByPlayer()
{
	if (m_doorState == DOOR_STATE_CLOSED)
	{
		if (material_instance_dynamic_ != NULL)	material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), loading_state_color);
		m_doorState = DOOR_STATE_LOADING;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 検知内のアクター数をインクリメント
		++door_eventbox_overlap_sum;

		// 挟み防止用フラグを立てる
		is_doorbody_eventbox_overlap = true;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// 検知内のアクター数をデクリメント
		--door_eventbox_overlap_sum;

		// 検知内のアクターが0以下なら挟み防止用フラグを下す
		if (door_eventbox_overlap_sum <= 0)		is_doorbody_eventbox_overlap = false;
	}
}