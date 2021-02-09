#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
	: enemy_state_(EState::kPatrol)
	, ptargetpoint_()
	, ppawnsensing_(NULL)
	, targetpoint_pos()
	, tp_index_(0)
	, in_eye_(false)
	, hitresult_(NULL)
	, headLine_(0.f)
	, attack_collision_(NULL)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ppawnsensing_ = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));	

	attack_collision_ = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	attack_collision_->SetupAttachment(RootComponent);

	is_player_damage = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	enemy_state_ = EState::kPatrol;

	// 視認された時に呼び出す関数
	Super::PostInitializeComponents();
	ppawnsensing_->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
	//ppawnsensing_->OnHearNoise.AddDynamic(this, &AEnemy::OnSeePlayer);

	attack_collision_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	// 設定したtargetpointの数分だけ座標の取得
	if (ptargetpoint_.Num() != 0)
	{
		for (int i = 0; i < ptargetpoint_.Num(); ++i)
		{
			targetpoint_pos.Add(ptargetpoint_[i]->GetActorLocation());
		}
	}

	// コントローラー取得(移動に使用)
	AIController = Cast<AEnemyMyAIController>(GetController());

	if (!is_launch_)
	{
		SetState(EState::kIdle);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (is_launch_ == false)
	{
		SetState(EState::kIdle);
		return;
	}

	if (attack_flag)
	{
		if (player_can_control_)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Idoukanou");
			if (Player != NULL)
			{
				Player->SetIsFound(false, FVector(0.f, 0.f, 0.f));
				if(is_player_damage)
				{
					Player->AttackFromEnemy();
					is_player_damage = false;
				}

			}
		}
	}

	if (enemy_state_ == EState::kIdle)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Idle");
		IdleCoolDown(DeltaTime);
	}
	else if (enemy_state_ == EState::kPatrol)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Patrol");
		Patrol(targetpoint_pos[tp_index_]);
		CheckMoveToTargetPoint();
	}
	else if (enemy_state_ == EState::kChase1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Hear");
		// NULLならプレイヤーが入る
		FVector enemyHeadPos = GetMesh()->GetSocketLocation("Head");

		GetWorld()->LineTraceSingleByChannel(hitresult_, enemyHeadPos, Player->GetActorLocation(), ECollisionChannel::ECC_Visibility);
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), enemyHeadPos, Player->GetActorLocation(), FLinearColor(0, 255, 0, 100), 1, 1);

		if (hitresult_.GetActor() == NULL)
		{
			Pursue_Chase();
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "NULL");
		}
		else
		{
			in_eye_ = false;
			OutSeePlayer();
		}
	}
	else if (enemy_state_ == EState::kHear)
	{
		Pursue_Hear();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Hear");
	}

	preb_attack_flag_ = attack_flag;
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
	// ログ
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "See");

	// プレイヤー取得

	Player = Cast<APlayerCharacter>(Pawn);
	if (Player != NULL)
	{
		in_eye_ = true;
		SetState(EState::kChase1);
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
}

void AEnemy::Pursue_Hear()
{
	AIController->MoveToLocation(noise_pos);
}

void AEnemy::OutSeePlayer()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "OutSee");
	SetState(EState::kIdle);
}

void AEnemy::OnHear(APawn* OtherActor, const FVector& Location, float Volume)
{
	SetState(EState::kHear);
}

void AEnemy::CheckMoveToTargetPoint()
{
	if (this->GetActorLocation().X >= targetpoint_pos[tp_index_].X - 100.f && this->GetActorLocation().X <= targetpoint_pos[tp_index_].X + 100)
	{
		if (this->GetActorLocation().Y >= targetpoint_pos[tp_index_].Y - 100.f && this->GetActorLocation().Y <= targetpoint_pos[tp_index_].Y + 100)
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

	if (OtherActor->ActorHasTag("Player"))
	{
		SetState(EState::kAttack);
		Player->SetIsFound(true, this->GetActorLocation());
		player_can_control_ = false;
		attack_flag = true;
		is_player_damage = true;
	}
}

void AEnemy::IdleCoolDown(float deltatime)
{
	time_cut_ += deltatime;
	if (time_cut_ >= idle_time_)
	{
		time_cut_ = 0.f;
		SetState(EState::kPatrol);
	}
}

void AEnemy::SetHearPos(FVector pos_)
{
	noise_pos = pos_;
}

void AEnemy::PlaySE() 
{
	if(enemy_state_ == EState::kChase1 && chase_se != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), chase_se, GetActorLocation());
	}
	else if (enemy_state_ == EState::kHear && hear_se != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), hear_se, GetActorLocation());
	}
}