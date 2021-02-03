// ---------------------------------------------
// ���� ����
// 2021/01/21 ~ 
// 
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
#include "Enemy.generated.h"

class AAIController;

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

	void SetState(EState ChangeState);
	void CheckMoveToTargetPoint();

public:
	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<ATargetPoint*> ptargetpoint_;

	UPROPERTY(VisibleAnywhere, Category = "pawnsensing")
		class UPawnSensingComponent* ppawnsensing_;

	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);

	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<FVector> targetpoint_pos;			// ���private����

	// �R���g���[���[�擾
	AEnemyMyAIController* AIController;
	// player�擾
	APlayerCharacter* Player;

	void Patrol(FVector pos_);
	void Pursue();
	void OutSeePlayer();

	int tp_index_;		// targetpoint�̗v�f��
	bool in_eye_;
	FHitResult hitresult_;	// ���C�L���X�g�̌���

	UPROPERTY(EditAnywhere, Category = "tp")
		float headLine_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EState enemy_state_;

	UPROPERTY(EditAnywhere, Category = "tp")
		UBoxComponent* attack_collision_;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle timer_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool attack_flag;		// �U���I�������m���邽�߂̃t���O

	bool preb_attack_flag_;
};
