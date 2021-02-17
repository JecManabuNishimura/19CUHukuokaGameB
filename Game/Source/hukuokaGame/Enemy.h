// targetPoint���i�[���Ȃ��ƃN���b�V��

// ---------------------------------------------
// ���� ����
// 2021/01/21 ~ 02/09
// �G��C++��
// ---------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"							// targetpoint���g�p���邽��
#include "Perception/PawnSensingComponent.h"			// pawnsensing���g�p���邽��
#include "Engine.h"										// ���O�n
#include "EnemyMyAIController.h"						// �R���g���[���[
#include "AIController.h"								// tp�ړ��ɂĎg�p
#include "PlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/Class.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"								// �U����SetTimer�֐�
#include "Containers/Array.h"							// Empty�`�F�b�N
#include "Math/Vector.h"								// �f�o�b�O�p
#include "Sound/SoundBase.h"
#include "Enemy.generated.h"

// �O���錾
class AAIController;

// �G�̃X�e�[�g�̐錾
UENUM(BlueprintType)
enum class EState : uint8
{
	kIdle UMETA(DisplayName = "Idle"),
	kPatrol UMETA(DisplayName = "Patrol"),
	kChase1 UMETA(DisplayName = "Chase1"), 
	kChase2 UMETA(DisplayName = "Chase2"),
	kHear UMETA(DisplayName = "Hear"),
	kAttack UMETA(DisplayName = "attack"),
};


UCLASS()
class HUKUOKAGAME_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	TArray<FVector> targetpoint_pos_;		// �Ή����Ă���targetpoint�̈ʒu���擾�p
	FVector last_see_pos;					// �Ō�Ɏ��F�������W

	void CheckMoveToTargetPoint();			// �ړ���ɓ����������m�F����֐�
	void IdleCoolDown(float _deltatime);	// Idle��Ԃ̉��������邩�m�F����֐�
	void Patrol(FVector _pos);		// Patrol��Ԃ̎��ɌĂ΂��֐�
	void Pursue_Chase();			// Chase��Ԃ̎��ɌĂ΂��֐�
	void Pursue_Hear();				// Hear��Ԃ̎��ɌĂ΂��֐�
	void OutSeePlayer();			// Chase��ԂŁAPlayer������̊O�ɂ���ꍇ�ɌĂ΂��֐�
	void PlaySE();					// SE��炷�֐�
	void LoseSight_Chase();			// ����������
	void CheckMoveToLastSeePos();	// ���������n�_�܂ōs�������ǂ������m�F����֐�


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool attack_flag_;							// �U���I�������m���邽�߂̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool is_launch_;							// �Q�[���J�n���ɓ������ėǂ����ǂ����̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool player_can_control_;					// �v���C���[�̑���\���ǂ����̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EState enemy_state_;						// �G�̏�Ԋi�[�p

	UPROPERTY(EditAnywhere, Category = "tp")
		float headLine_;							// ���̍����̐ݒ�p(���C���΂������ɂĎg�p)

	UPROPERTY(EditAnywhere, Category = "tp")
		float idle_time_;							// �ҋ@����

	UPROPERTY(EditAnywhere, Category = "tp")
		float chase_end_length_;					// �ǐՏI���������鋗��

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector noise_pos_;							// ���𔭂����n�_

	UPROPERTY(VisibleAnywhere, Category = "pawnsensing")
		class UPawnSensingComponent* ppawnsensing_;	// pawnsensing�i�[�p

	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<ATargetPoint*> ptargetpoint_;		// targetpoint�i�[�p

			// �U���̓����蔻��
	UPROPERTY(EditAnywhere, Category = "tp")
		UBoxComponent* attack_collision_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* hear_se_;						// �������m�����ۂɖ炷SE�i�[�p

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* chase_se_;						// ���o�Ō��m�����ۂɖ炷SE�i�[�p

// ----------------------------------------------------------------------------------------

	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);											// �v���C���[�����F�����ۂɌĂ΂��

	UFUNCTION()
		void OnHear(APawn* OtherActor, const FVector& Location, float Volume);	// �����̖ڏ��������ĂȂ�

	// attack_collision_�ɂĎg�p
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Set")								// �O���I�u�W�F�N�g�ɂĉ��𔭂����n�_���i�[����֐�
		void SetHearPos(FVector _pos);

	UFUNCTION(BlueprintCallable, Category = "Set")
		void SetState(EState _changeState);

// ----------------------------------------------------------------------------------------

	bool in_eye_;				// ����̒��ɂ��邩�ǂ���
	bool chase_flag_;			// ���F�����ۂɂ��Ă�t���O
	bool is_player_damage_;		// �_���[�W���󂯂��Ԃ��ۂ����m�F����֐�(���ꂪ������tick�֐��ŉ��x���Ă΂�Ă��܂����ߍ쐬)
	FHitResult hitresult_;		// ���C�L���X�g�̌���
	float time_cut_;			// Idle��Ԃ̌o�ߎ��Ԋi�[�p
	int tp_index_;				// targetpoint�̗v�f��

	AEnemyMyAIController* AIController;		// �R���g���[���[�擾
	APlayerCharacter* Player;				// player�擾
};