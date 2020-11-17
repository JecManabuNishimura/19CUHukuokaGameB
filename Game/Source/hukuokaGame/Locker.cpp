// max_rotationを0以上にするとおかしくなります
// 181°~360°にするとおかしくなる
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
	, max_rotation(0)
	, open_and_close_frame(0)
	, player_change_rotation_frame(0)
	, player_to_locker_frame(0)
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
	, locker_body_rotation_(FRotator::ZeroRotator)
	, location_lerp_alpha(0.f)
	, location_add_lerp_value(0.f)
	, rotation_lerp_alpha(0.f)
	, rotation_add_lerp_value(0.f)
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

	if (max_rotation == 0 || open_and_close_frame == 0 || player_change_rotation_frame == 0 || player_to_locker_frame == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Propety is 0. Should be Another Value"));
	}
}

void ALocker::BeginPlay()
{
	Super::BeginPlay();

	// 1フレームごとにどれだけ回転させるかの設定(1フレームに与える回転量 = 回転させたい最大の角度 / かかるF数)
	add_rotation_value_ = (float)(max_rotation / open_and_close_frame);

	def_door_rotation = locker_body_rotation_.Yaw;

	locker_body_location_ = body_mesh_->GetRelativeLocation();
	locker_body_rotation_ = body_mesh_->GetRelativeRotation();
	//max_rotation += locker_body_rotation_.Yaw;
	//now_rotation_value_ = locker_body_rotation_.Yaw;
	locker_body_rotation_.Yaw += 90.f;
	//add_player_rotation_value_ = locker_body_rotation_.Yaw / player_change_rotation_frame;

	rotation_add_lerp_value = 1.f / (float)player_change_rotation_frame;

	GetPlayer();
}

void ALocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	is_end_rotation_ = false;
	// 扉の開く・閉じるときの角度の値の設定
	SetDoorRotationValue();

	// 角度の反映
	UpdateDoorRotation();

	// プレイヤーがロッカーの中に入る・出る処理
	if (!is_in_player_)
	{
		InToLocker();
	}
	else
	{
		OutToLocker();
	}

	if (player_rotation_start_flag_)
	{
		PlayerRotation();
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

	is_check_ = !is_check_;
	can_input_ = false;
	// プレイヤーがロッカーに移動する際の1F辺りの移動量 = (ロッカーの座標 - Playerの現在地) / 総フレーム数

	if (!is_in_player_)
	{
		player_location_save_ = player->GetActorLocation();

		location_lerp_alpha = 0.f;
		player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
		location_add_lerp_value = 1.f / (float)player_to_locker_frame;
		locker_body_location_.Z = player_location_save_.Z;

		rotation_lerp_alpha = 0.f;
	}
	else
	{
		location_lerp_alpha = 1.f;
		//player_move_vector_ = (player->GetActorLocation() - player_location_save_) / player_to_locker_frame;
	}

	player->SetPlayerControlFlag(false);
}

void ALocker::SetDoorRotationValue()
{
	// ロッカーの扉を開けるフラグがたつ
	if (is_check_)
	{
		// 開ききっていないかどうか
		if (now_rotation_value_ >= max_rotation)
		{
			now_rotation_value_ += add_rotation_value_;

			// max_rotationを超えているなら補正(多分要らないけど)
			if (now_rotation_value_ <= max_rotation)
			{
				now_rotation_value_ = max_rotation;
				is_end_rotation_ = true;
			}
		}
	}
	else
	{
		// 閉まり切っていないかどうか
		if (now_rotation_value_ < 0)
		{
			now_rotation_value_ -= add_rotation_value_;

			// 0未満なら補正(多分要らないけど)
			if (now_rotation_value_ >= 0)
			{
				now_rotation_value_ = 0.f;
				// 閉まるときは要らない
				//is_end_rotation_ = true;			
			}
		}
	}
}

void ALocker::UpdateDoorRotation()
{
	door_mesh_->SetRelativeRotation(FRotator(0.f, now_rotation_value_, 0.f));
}

void ALocker::InToLocker()
{
	// 扉が回転し終えていない かつ 回転処理が終了しきっていないなら処理させない
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Playerの移動中の移動中な為trueに
	is_move_in_locker_ = true;

	location_lerp_alpha += location_add_lerp_value; 
	player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
	player->SetActorLocation(player_move_vector_);
	
	if (location_lerp_alpha >= 0.99f)
	{
		player_rotation_start_flag_ = true;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = true;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
}

void ALocker::OutToLocker()
{
	// 扉が回転し終えていない かつ 回転処理が終了しきっていないなら処理させない
	if (!is_end_rotation_ && !is_move_in_locker_)
	{
		return;
	}

	// Playerの移動中の移動中な為trueに
	is_move_in_locker_ = true;

	location_lerp_alpha -= location_add_lerp_value;
	player_move_vector_ = FMath::Lerp(player_location_save_, locker_body_location_, location_lerp_alpha);
	player->SetActorLocation(player_move_vector_);

	if(location_lerp_alpha <= 0)
	{
		player_rotation_start_flag_ = false;
		is_move_in_locker_ = false;
		is_check_ = false;
		is_in_player_ = false;
		player->SetPlayerControlFlag(true);
		can_input_ = true;
	}
}

void ALocker::PlayerRotation()
{
		
	rotation_lerp_alpha += rotation_add_lerp_value;
	player->SetActorRotation(FMath::Lerp(player->GetActorRotation(), locker_body_rotation_, rotation_lerp_alpha));

	if (rotation_lerp_alpha >= 0.99f)
	{
		player_rotation_start_flag_ = false;
	}
}

void ALocker::CloseDoor()
{

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