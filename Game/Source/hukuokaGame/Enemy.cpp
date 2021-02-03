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
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	enemy_state_ = EState::kPatrol;

	// 視認された時に呼び出す関数
	ppawnsensing_->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);

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
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (attack_flag == false)
	{
		if (attack_flag == false && preb_attack_flag_ == true)
		{
			Player->SetIsFound(false, FVector(0.f, 0.f, 0.f));
			Player->AttackFromEnemy();
		}
	}

	if (enemy_state_ == EState::kIdle)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Idle");
	}
	else if (enemy_state_ == EState::kPatrol)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Patrol");
		Patrol(targetpoint_pos[tp_index_]);
		CheckMoveToTargetPoint();
	}
	else if (enemy_state_ == EState::kChase1)
	{
		// NULLならプレイヤーが入る
		FVector enemyHeadPos = GetMesh()->GetSocketLocation("Head");

		GetWorld()->LineTraceSingleByChannel(hitresult_, enemyHeadPos, Player->GetActorLocation(), ECollisionChannel::ECC_Visibility);
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), enemyHeadPos, Player->GetActorLocation(), FLinearColor(0, 255, 0, 100), 1, 1);

		if (hitresult_.GetActor() == NULL)
		{
			in_eye_ = true;
			Pursue();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "NULL");
		}
		else
		{
			in_eye_ = false;
		}
	}
	
	if (in_eye_ == false)
	{
		OutSeePlayer();
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
	enemy_state_ = ChangeState;
}

void AEnemy::Patrol(FVector pos_)
{
	AIController->MoveToLocation(pos_);
}

void AEnemy::Pursue()
{
	AIController->MoveToActor(Player);
}

void AEnemy::OutSeePlayer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "OutSee");
	//SetState(EState::kIdle);
}

void AEnemy::CheckMoveToTargetPoint()
{
	if (this->GetActorLocation() == targetpoint_pos[tp_index_])
	{
		++tp_index_;
		tp_index_ = tp_index_ % ptargetpoint_.Num();
	}
}

void AEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		SetState(EState::kAttack);
		Player->SetIsFound(true, this->GetActorLocation());
		attack_flag = true;
	}
}
