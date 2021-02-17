#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
	: targetpoint_pos_()
	, last_see_pos(FVector::ZeroVector)
	, attack_flag_(false)
	, is_launch_(false)
	, player_can_control_(true)
	, enemy_state_(EState::kPatrol)
	, headLine_(0.f)
	, idle_time_(0.f)
	, chase_end_length_(0.f)
	, noise_pos_(FVector::ZeroVector)
	, ppawnsensing_(NULL)
	, ptargetpoint_()
, attack_collision_(NULL)
, hear_se_(NULL)
, chase_se_(NULL)
, in_eye_(false)
, chase_flag_(false)
, is_player_damage_(false)
, hitresult_(NULL)
, time_cut_(0.f)
, tp_index_(0)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ppawnsensing_ = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	attack_collision_ = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	attack_collision_->SetupAttachment(RootComponent);

	is_player_damage_ = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	enemy_state_ = EState::kPatrol;

	// 視認された時に呼び出す関数
	Super::PostInitializeComponents();
	ppawnsensing_->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
	// 実装が上手くできない為BPに記入
	//ppawnsensing_->OnHearNoise.AddDynamic(this, &AEnemy::OnSeePlayer);

	attack_collision_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	// 設定したtargetpointの数分だけ座標の取得
	if (ptargetpoint_.Num() != 0)
	{
		for (int i = 0; i < ptargetpoint_.Num(); ++i)
		{
			targetpoint_pos_.Add(ptargetpoint_[i]->GetActorLocation());
		}
	}

	// コントローラー取得(移動に使用)
	AIController = Cast<AEnemyMyAIController>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 起動可能でないならIdle状態にしてretrunする
	if (is_launch_ == false)
	{
		SetState(EState::kIdle);
		return;
	}

	// 攻撃状態になり、攻撃のフラグが立てられたとき
	if (attack_flag_)
	{
		// プレイヤーが動かせるようになったら
		if (player_can_control_)
		{
			if (Player != NULL)
			{
				// プレイヤー側を捕まっていない状態
				Player->SetIsFound(false, FVector(0.f, 0.f, 0.f));
				// 一度だけダメージを与える
				if (is_player_damage_)
				{
					Player->AttackFromEnemy();
					is_player_damage_ = false;
				}

			}
		}
	}
	// 待機状態
	if (enemy_state_ == EState::kIdle)
	{
		IdleCoolDown(DeltaTime);		// 一定時間待機
	}
	// 巡回状態
	else if (enemy_state_ == EState::kPatrol)
	{
		Patrol(targetpoint_pos_[tp_index_]);		// 指定された地点を徘徊
		CheckMoveToTargetPoint();					// 目的地に着いたかチェックする関数
	}
	// 追跡状態
	else if (enemy_state_ == EState::kChase1 || enemy_state_ == EState::kChase2)
	{
		// NULLならプレイヤーが入る
		FVector enemyHeadPos = GetMesh()->GetSocketLocation("Head");

		// UKismetSystemLibrary::DrawDebugLine(GetWorld(), enemyHeadPos, Player->GetCameraLocation(), FLinearColor(0, 255, 0, 100), 1, 1);
		UE_LOG(LogTemp, Warning, TEXT("distance = %f"), (GetActorLocation() - Player->GetActorLocation()).Size());
		if (chase_end_length_ > (GetActorLocation() - Player->GetActorLocation()).Size())
		{
			// レイを飛ばしてプレイヤーが隠れる様であれば
			if (GetWorld()->LineTraceSingleByChannel(hitresult_, enemyHeadPos, Player->GetCameraLocation(), ECollisionChannel::ECC_Visibility))
			{
				LoseSight_Chase();
				CheckMoveToLastSeePos();
			}
			else
			{
				Pursue_Chase();
			}
		}
		else
		{
			in_eye_ = false;
			OutSeePlayer();
		}



	}
	// 聴覚検知状態
	else if (enemy_state_ == EState::kHear)
	{
		// 音の鳴るほうへ移動
		Pursue_Hear();
	}

	// 直前まで追跡を行っていた場合
	if (enemy_state_ != EState::kChase1 && enemy_state_ != EState::kChase2)
	{
		if (chase_flag_ == true)
		{
			if (Player != NULL)
			{
				chase_flag_ = false;
				Player->SetEnemyChased(false);
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OnSeePlayer (APawn* Pawn)
{
	if (enemy_state_ == EState::kAttack)
	{
		return;
	}

	// プレイヤー取得

	Player = Cast<APlayerCharacter>(Pawn);
	if (Player != NULL)
	{
		in_eye_ = true;
		SetState(EState::kChase1);
		Player->SetEnemyChased(true);
		chase_flag_ = true;
	}
}

void AEnemy::SetState(EState ChangeState)
{
	if (enemy_state_ == ChangeState)
	{
		return;
	}

	enemy_state_ = ChangeState;
	PlaySE();
}

void AEnemy::Patrol(FVector pos_)
{
	AIController->MoveToLocation(pos_);
}

void AEnemy::Pursue_Chase()
{
	AIController->MoveToActor(Player);
	last_see_pos = Player->GetActorLocation();
}

void AEnemy::Pursue_Hear()
{
	AIController->MoveToLocation(noise_pos_);
}

void AEnemy::OutSeePlayer()
{
	SetState(EState::kIdle);
	Player->SetEnemyChased(false);
}

void AEnemy::OnHear(APawn* OtherActor, const FVector& Location, float Volume)
{
	SetState(EState::kHear);
}

void AEnemy::CheckMoveToTargetPoint()
{
	if (this->GetActorLocation().X >= targetpoint_pos_[tp_index_].X - 200.f && this->GetActorLocation().X <= targetpoint_pos_[tp_index_].X + 200)
	{
		if (this->GetActorLocation().Y >= targetpoint_pos_[tp_index_].Y - 200.f && this->GetActorLocation().Y <= targetpoint_pos_[tp_index_].Y + 200)
		{
			++tp_index_;
			tp_index_ = tp_index_ % ptargetpoint_.Num();
			SetState(EState::kIdle);
		}
	}
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!is_launch_)
	{
		return;
	}

	if (OtherActor->ActorHasTag("Player") && enemy_state_ != EState::kAttack)
	{
		SetState(EState::kAttack);
		Player->SetIsFound(true, this->GetActorLocation());
		player_can_control_ = false;
		attack_flag_ = true;
		is_player_damage_ = true;
	}
}

void AEnemy::IdleCoolDown(float _deltatime)
{
	time_cut_ += _deltatime;
	if (time_cut_ >= idle_time_)
	{
		time_cut_ = 0.f;
		SetState(EState::kPatrol);
	}
}

void AEnemy::SetHearPos(FVector _pos)
{
	noise_pos_ = _pos;
}

void AEnemy::PlaySE() 
{
	if(enemy_state_ == EState::kChase1 && chase_se_ != NULL)
	{
		if (chase_end_length_ > (GetActorLocation() - Player->GetActorLocation()).Size())
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), chase_se_, GetActorLocation());
		}
	}
	else if (enemy_state_ == EState::kHear && hear_se_ != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), hear_se_, GetActorLocation());
	}
}

void AEnemy::LoseSight_Chase()
{
	AIController->MoveToLocation(last_see_pos);
}

void AEnemy::CheckMoveToLastSeePos()
{
	//FVector a = this->GetActorLocation();
	//UE_LOG(LogTemp, Warning, TEXT("a( X : %f, Y : %f, Z : %f)"), a.X, a.Y, a.Z);
	//UE_LOG(LogTemp, Warning, TEXT("a( X : %f, Y : %f, Z : %f)"), last_see_pos.X, last_see_pos.Y, last_see_pos.Z);
	if (this->GetActorLocation().X >= last_see_pos.X - 100.f && this->GetActorLocation().X <= last_see_pos.X + 100)
	{
		if (this->GetActorLocation().Y >= last_see_pos.Y - 100.f && this->GetActorLocation().Y <= last_see_pos.Y + 100)
		{
			in_eye_ = false;
			OutSeePlayer();
		}
	}
}