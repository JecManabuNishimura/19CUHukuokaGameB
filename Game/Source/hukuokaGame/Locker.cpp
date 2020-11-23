// max_rotation_ > 0にしない
// 配置の際はBP_Lockerを置くこと
//-------------------------------------------------------------------
// ファイル		：Locker.cpp
// 概要			：ロッカーの制御
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/11/16
//-------------------------------------------------------------------

#include "Locker.h"

ALocker::ALocker()
	: body_mesh_(NULL)
	, door_mesh_(NULL)
	, max_rotation_(0)
	, open_and_close_second_(0.f)
	, player_change_rotation_second_(0.f)
	, player_to_locker_second_(0.f)
	, flont_distance_(0)
	, player(NULL)
	, is_end_rotation_(false)
	, can_input_(true)
	, is_check_(false)
	, is_move_in_locker_(false)
	, is_in_player_(false)
	, player_rotation_start_flag_(false)
	, add_rotation_value_(0.f)
	, now_rotation_value_(0.f)
	, add_player_rotation_value_(0.f)
	, locker_body_location_(FVector::ZeroVector)
	, player_location_save_(FVector::ZeroVector)
	, player_move_vector_(FVector::ZeroVector)
	, body_front_location_(FVector::ZeroVector)
	, locker_body_rotation_(FRotator::ZeroRotator)
	, location_lerp_alpha1_(0.f)
	, location_lerp_alpha2_(0.f)
	, location_add_lerp_value_(0.f)
	, rotation_lerp_alpha_(0.f)
	, rotation_add_lerp_value_(0.f)
{
	// ティックを呼び出すかのフラグ
	PrimaryActorTick.bCanEverTick = true;

	// 各メッシュの設定(コンポーネントとして追加)
	body_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("body_mesh_"));
	if (body_mesh_ != NULL)
	{
		RootComponent = body_mesh_;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Locker.cpp body_mesh_ is NULL"));
	}

	door_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("door_mesh_"));
	if (door_mesh_ != NULL)
	{
		door_mesh_->SetupAttachment(body_mesh_);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Locker.cpp door_mesh_ is NULL"));
	}
}

void ALocker::BeginPlay()
{
	Super::BeginPlay();

	// 1秒ごとにどれだけ回転させるかの設定(1秒に与える回転量 = 回転させたい最大の角度 / かかる秒数)
	add_rotation_value_ = (float)(max_rotation_) / open_and_close_second_;

	// 起動時のロッカー本体のZ軸を格納
	def_door_rotation_ = locker_body_rotation_.Yaw;

	// player_location_save_->door_front_location と body_front_location_->locker_body_location_で移動させる為、2で割る
	location_add_lerp_value_ = 2.f / (float)player_to_locker_second_;

	// ロッカー本体の座標・角度を格納し、角度にはZ軸のみ+90する(でないとロッカーにPlayerが入った際に意図しない方向を向くため)
	locker_body_location_ = body_mesh_->GetRelativeLocation();
	locker_body_rotation_ = body_mesh_->GetRelativeRotation();
	locker_body_rotation_.Yaw += 90.f;

	// ロッカーの正面の座標を格納(flont_distance_で調整可)
	body_front_location_ = GetActorRightVector() * flont_distance_ + GetActorLocation();

	// 1F辺りに加算する値 = 1 / プレイヤーがロッカーの中で回転する総秒数
	// 1 = (lerpの最大値)
	rotation_add_lerp_value_ = 1.f / player_change_rotation_second_;

	GetPlayer();
}

void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 扉の回転終了フラグの初期化
	is_end_rotation_ = false;

	// 扉の開く・閉じるときの角度の値の設定
	SetDoorRotationValue(DeltaTime);

	// 角度の反映
	UpdateDoorRotation(DeltaTime);

	// プレイヤーがロッカーの中に入る・出る処理
	if (!is_in_player_)
	{
		// 中に入る
		InToLocker(DeltaTime);
	}
	else
	{
		// 出る
		OutToLocker(DeltaTime);
	}

	// プレイヤーがロッカーの中に入ってフラグが立てられたら
	if (player_rotation_start_flag_)
	{
		PlayerRotation(DeltaTime);
	}
}

ALocker::~ALocker()
{

}

void ALocker::CheckedByPlayer()
{
	// 入力可能状態か
	if (!can_input_)
	{
		return;
	}

	// 入力可能な為チェック状態の反転とロッカーに対する入力受け付けを無効に
	is_check_ = !is_check_;
	can_input_ = false;

	// ボタンが押された際にプレイヤーがロッカーの中にいるかどうか
	if (!is_in_player_)
	{
		// 現在位置の保存
		player_location_save_ = player->GetActorLocation();

		// lerpに使う値の初期化
		location_lerp_alpha1_ = 0.f;		// player_location_save_ -> body_front_location_に使う leap
		location_lerp_alpha2_ = 0.f;		// body_front_location_  -> locker_body_location_に使うlerp
		rotation_lerp_alpha_ = 0.f;		// ロッカーに入った際の回転に使ってるlerp

		//ロッカー本体とロッカーの前にある座標のZ軸はplayerのZ軸に固定(Beginでもいいのかもしれない)
		locker_body_location_.Z = player_location_save_.Z;
		body_front_location_.Z = player_location_save_.Z;

		// 中に入るフラグを立てる
		// ここでPlayerの制御に制限かけてる
		player->SetInTheLocker(true);
		player->SetPlayerControlFlag(false);
	}
	else
	{
		// lerpに使う値を最大値で初期化
		location_lerp_alpha1_ = 1.f;
		location_lerp_alpha2_ = 1.f;
	}
}

void ALocker::SetDoorRotationValue(float DeltaTime)
{
	// ロッカーの扉を開けるフラグがたつ
	if (is_check_)
	{
		// 開ききっていないかどうか(不等号いらんかも)
		if (now_rotation_value_ >= max_rotation_)
		{
			now_rotation_value_ += add_rotation_value_ * DeltaTime;

			// max_rotation_を超えているなら補正・回転終了フラグを立てる
			if (now_rotation_value_ <= max_rotation_)
			{
				now_rotation_value_ = max_rotation_;
				is_end_rotation_ = true;
			}
		}
	}
	else
	{
		// 閉まり切っていないかどうか
		if (now_rotation_value_ < 0)
		{
			now_rotation_value_ -= add_rotation_value_ * DeltaTime;

			// 0未満なら補正・回転終了フラグを立てる
			if (now_rotation_value_ >= 0)
			{
				now_rotation_value_ = 0.f;
				// 閉まるときは要らない(移動してしまう為)
				//is_end_rotation_ = true;			
			}
		}
	}
}

void ALocker::UpdateDoorRotation(float DeltaTime)
{
	// SetDoorRotationValue()で設定した値を反映
	door_mesh_->SetRelativeRotation(FRotator(0.f, now_rotation_value_, 0.f));
}

void ALocker::InToLocker(float DeltaTime)
{
	// 扉が回転し終えていない かつ 回転処理が終了しきっていないなら処理させない
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Playerの移動中の移動中な為trueに
	is_move_in_locker_ = true;

	// player_location_save_ -> body_front_location_ の移動が(ほぼ)終わったら
	if (location_lerp_alpha1_ >= 0.99f)
	{
		// body_front_location_  -> locker_body_location_ の移動(lerpのalpha値の設定)
		location_lerp_alpha2_ += location_add_lerp_value_ * DeltaTime;

		// Playerの座標の更新
		player_move_vector_ = FMath::Lerp(body_front_location_, locker_body_location_, location_lerp_alpha2_);
		player->SetActorLocation(player_move_vector_);

		// body_front_location_  -> locker_body_location_ の移動が(ほぼ)終わったら
		if (location_lerp_alpha2_ >= 0.99f)
		{
			player_rotation_start_flag_ = true;		// Playerの回転処理のフラグを立てる
			is_move_in_locker_ = false;				// ロッカーの移動が終了したのでfalse
			is_check_ = false;						// 調べた状態の解除
			is_in_player_ = true;					// ロッカーの中にいるのでtrue
			//player->SetPlayerControlFlag(true);		// いらんかも
			can_input_ = true;						// 入力受け付けを可能に
		}
	}
	// player_location_save_ -> body_front_location_ の移動が終わってない
	else
	{
		// player_location_save_->body_front_location_ の移動(lerpのalpha値の設定)
		location_lerp_alpha1_ += location_add_lerp_value_ * DeltaTime;

		// Playerの座標の更新
		player_move_vector_ = FMath::Lerp(player_location_save_, body_front_location_, location_lerp_alpha1_);

		player->SetActorLocation(player_move_vector_);
	}
}

void ALocker::OutToLocker(float DeltaTime)
{
	// 扉が回転し終えていない かつ 回転処理が終了しきっていないなら処理させない
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Playerの移動中の移動中な為trueに
	is_move_in_locker_ = true;

	// body_front_location_  -> locker_body_location_ の移動(lerpのalpha値の設定)
	location_lerp_alpha2_ -= location_add_lerp_value_ * DeltaTime;

	// Playerの座標の更新
	player_move_vector_ = FMath::Lerp(body_front_location_, locker_body_location_, location_lerp_alpha2_);
	player->SetActorLocation(player_move_vector_);

	// body_front_location_  -> locker_body_location_ の移動が(ほぼ)終わったら
	if(location_lerp_alpha2_ <= 0)
	{
		player_rotation_start_flag_ = false;	// Playerの回転処理はさせない為false
		is_move_in_locker_ = false;				// ロッカーの移動が終了したのでfalse
		is_check_ = false;						// 調べた状態の解除
		is_in_player_ = false;					// ロッカーの中にいないのでfalse
		can_input_ = true;						// 入力受け付けを可能に
		player->SetInTheLocker(false);			// Playerをロッカーの中にいない状態に
		player->SetPlayerControlFlag(true);
	}
}

void ALocker::PlayerRotation(float DeltaTime)
{
	// Playerの現在の角度 -> locker_body_rotation_へ回転
	rotation_lerp_alpha_ += rotation_add_lerp_value_* DeltaTime;
	player->SetActorRotation(FMath::Lerp(player->GetActorRotation(), locker_body_rotation_, rotation_lerp_alpha_));

	// 回転し終えたか
	if (rotation_lerp_alpha_ >= 0.99f)
	{
		// 回転させないように
		player_rotation_start_flag_ = false;
	}
}

void ALocker::GetPlayer()
{
	TSubclassOf<APlayerCharacter> findClass = APlayerCharacter::StaticClass();
	TArray<AActor*> pPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, pPlayers);
	if (pPlayers.Num())
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(pPlayers[0]);
		FString message = FString("PlayerName") + pPlayer->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, message);

		player = pPlayer;
	}
}