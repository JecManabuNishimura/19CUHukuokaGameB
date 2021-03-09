#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
	: last_see_pos(FVector::ZeroVector)
	, attack_flag_(false)
	, is_launch_(false)
	, player_can_control_(true)
	, enemy_state_(EState::kPatrol)
	, attackcollision_enable_time_(0.f)
	, sight_radius_(5000.f)	// �쐬����pawnsensing�̃f�t�H�̐ݒ�
	, chase_se_interval_(0.f)
	, headLine_(0.f)
	, idle_time_(0.f)
	, chase_end_length_(0.f)
	, stuck_time_(0.f)
	, noise_pos_(FVector::ZeroVector)
	, ppawnsensing_(NULL)
	, ptargetpoint_()
	, targetpoint_pos_()
	, attack_collision_(NULL)
	, hear_se_(NULL)
	, chase_se_(NULL)
	, in_eye_(false)
	, chase_flag_(false)
	, is_player_damage_(false)
	, hitresult_(NULL)
	, preb_pos_(FVector::ZeroVector)
	, attackcollision_enable_time_cnt(0.f)
	, chase_se_cnt(0.f)
	, idle_time_cut_(0.f)
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

	// ���F���ꂽ���ɌĂяo���֐�
	Super::PostInitializeComponents();
	ppawnsensing_->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePlayer);
	// ��������肭�ł��Ȃ���BP�ɋL��
	//ppawnsensing_->OnHearNoise.AddDynamic(this, &AEnemy::OnSeePlayer);

	attack_collision_->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnOverlapBegin);

	// �ݒ肵��targetpoint�̐����������W�̎擾
	if (ptargetpoint_.Num() != 0)
	{
		for (int i = 0; i < ptargetpoint_.Num(); ++i)
		{
			targetpoint_pos_.Add(ptargetpoint_[i]->GetActorLocation());
		}
	}

	// �R���g���[���[�擾(�ړ��Ɏg�p)
	AIController = Cast<AEnemyMyAIController>(GetController());

	if (is_launch_ == false)
	{
		SetState(EState::kIdle);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �N���\�łȂ��Ȃ�Idle��Ԃɂ���retrun����
	if (is_launch_ == false)
	{
		return;
	}

	if (!attack_collision_ -> IsCollisionEnabled())
	{
		AttackCoolDown(DeltaTime);
	}

	// �U����ԂɂȂ�A�U���̃t���O�����Ă�ꂽ�Ƃ�
	if (attack_flag_)
	{
		// �v���C���[����������悤�ɂȂ�����
		if (player_can_control_)
		{
			if (Player != NULL)
			{
				// �v���C���[����߂܂��Ă��Ȃ����
				Player->SetIsFound(false, FVector(0.f, 0.f, 0.f));
				// ��x�����_���[�W��^����
				if (is_player_damage_)
				{
					Player->AttackFromEnemy();
					is_player_damage_ = false;
				}

			}
		}
	}
	// �ҋ@���
	if (enemy_state_ == EState::kIdle)
	{
		UE_LOG(LogTemp, Warning, TEXT("State::Idle"));
		IdleCoolDown(DeltaTime);		// ��莞�ԑҋ@
		chase_se_cnt = 0.f;
		ppawnsensing_->SightRadius = sight_radius_;
	}
	// ������
	else if (enemy_state_ == EState::kPatrol)
	{
		UE_LOG(LogTemp, Warning, TEXT("State::Patrol"));
		Patrol(targetpoint_pos_[tp_index_]);		// �w�肳�ꂽ�n�_��p�j
		CheckMoveToTargetPoint();					// �ړI�n�ɒ��������`�F�b�N����֐�
		chase_se_cnt = 0.f;
		ppawnsensing_->SightRadius = sight_radius_;
	}
	// �ǐՏ��
	else if (enemy_state_ == EState::kChase1 || enemy_state_ == EState::kChase2)
	{
		if (enemy_state_ == EState::kChase1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Chase1"));
		}
		if (enemy_state_ == EState::kChase2)
		{
			UE_LOG(LogTemp, Warning, TEXT("Chase2"));
		}

		// NULL�Ȃ�v���C���[������
		FVector enemyHeadPos = GetMesh()->GetSocketLocation("Head");

		if (Player != NULL)
		{
			// UKismetSystemLibrary::DrawDebugLine(GetWorld(), enemyHeadPos, Player->GetCameraLocation(), FLinearColor(0, 255, 0, 100), 1, 1);
			UE_LOG(LogTemp, Warning, TEXT("distance = %f"), (GetActorLocation() - Player->GetActorLocation()).Size());
			// ���C���΂��ăv���C���[���B���l�ł����
			if (GetWorld()->LineTraceSingleByChannel(hitresult_, enemyHeadPos, Player->GetCameraLocation(), ECollisionChannel::ECC_Visibility))
			{
				if (chase_end_length_ > (GetActorLocation() - Player->GetActorLocation()).Size())
				{
					UE_LOG(LogTemp, Warning, TEXT("loseSight"));
					LoseSight_Chase();
					CheckMoveToLastSeePos();
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("OutseePlayer"));
					in_eye_ = false;
					OutSeePlayer();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Chase"));
				Pursue_Chase();
				PlayIntervalSE(DeltaTime);
			}
		}
		

		CheckIsStuck(DeltaTime);
		preb_pos_ = this->GetActorLocation();
	}

	// ���o���m���
	else if (enemy_state_ == EState::kHear)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hear"));
		// ���̖�ق��ֈړ�
		Pursue_Hear();
		CheckIsStuck(DeltaTime);
		preb_pos_ = this->GetActorLocation();
	}

	// ���O�܂ŒǐՂ��s���Ă����ꍇ
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

	// �v���C���[�擾

	Player = Cast<APlayerCharacter>(Pawn);
	if (Player != NULL)
	{
		in_eye_ = true;
		if (enemy_state_ != EState::kChase1 && enemy_state_ != EState::kChase2)
		{
			SetState(EState::kChase1);
			Player->SetEnemyChased(true);
			chase_flag_ = true;
		}
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
	if (Player != NULL)
	{
		AIController->MoveToActor(Player);
		last_see_pos = Player->GetActorLocation();
	}
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
	idle_time_cut_ += _deltatime;
	if (idle_time_cut_ >= idle_time_)
	{
		idle_time_cut_ = 0.f;
		SetState(EState::kPatrol);
	}
}

void AEnemy::SetHearPos(FVector _pos)
{
	noise_pos_ = _pos;
}

void AEnemy::PlaySE() 
{
	if (Player != NULL)
	{
		if (enemy_state_ == EState::kChase1 && chase_se_ != NULL)
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
	
}

void AEnemy::PlayIntervalSE(float _deltatime)
{
	if (chase_se_cnt >= chase_se_interval_ && chase_se_ != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), chase_se_, GetActorLocation());
		chase_se_cnt = 0.f;
	}
	else
	{
		chase_se_cnt += _deltatime;
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

void AEnemy::CheckIsStuck(float _deltatime)
{
	if (enemy_state_ == EState::kChase1)
	{
		UE_LOG(LogTemp, Warning, TEXT("retrun"));
		return;
	}

	if (preb_pos_ == this->GetActorLocation())
	{
		stuck_time_cnt_ += _deltatime;
		UE_LOG(LogTemp, Warning, TEXT("++"));
		if (stuck_time_cnt_ >= stuck_time_)
		{
			stuck_time_cnt_ = 0.f;
			SetState(EState::kPatrol);
		}
	}
	else
	{
		stuck_time_cnt_ = 0.f;
	}
}

void AEnemy::AttackCoolDown(float _deltatime)
{
	attackcollision_enable_time_cnt += _deltatime;

	if (attackcollision_enable_time_cnt >= attackcollision_enable_time_)
	{
		attack_collision_->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		attackcollision_enable_time_cnt = 0.f;
	}
}