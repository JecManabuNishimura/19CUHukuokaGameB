// targetPointを格納しないとクラッシュ

// ---------------------------------------------
// 尾崎 蒼宙
// 2021/01/21 ~ 02/09
// 敵のC++化
// ---------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"							// targetpointを使用するため
#include "Perception/PawnSensingComponent.h"			// pawnsensingを使用するため
#include "Engine.h"										// ログ系
#include "EnemyMyAIController.h"						// コントローラー
#include "AIController.h"								// tp移動にて使用
#include "PlayerCharacter.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/Class.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"								// 攻撃のSetTimer関数
#include "Containers/Array.h"							// Emptyチェック
#include "Math/Vector.h"								// デバッグ用
#include "Sound/SoundBase.h"
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

	
	void CheckMoveToTargetPoint();
	void IdleCoolDown(float deltatime_);

public:
	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<ATargetPoint*> ptargetpoint_;

	UPROPERTY(VisibleAnywhere, Category = "pawnsensing")
		class UPawnSensingComponent* ppawnsensing_;

	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);
	UFUNCTION()
		void OnHear(APawn* OtherActor, const FVector&Location, float Volume);

	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<FVector> targetpoint_pos;			// 後でprivate推奨

	// コントローラー取得
	AEnemyMyAIController* AIController;
	// player取得
	APlayerCharacter* Player;

	void Patrol(FVector pos_);
	void Pursue_Chase();
	void Pursue_Hear();
	void OutSeePlayer();

	int tp_index_;		// targetpointの要素数
	bool in_eye_;
	FHitResult hitresult_;	// レイキャストの結果

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
	bool attack_flag;		// 攻撃終了を検知するためのフラグ

	bool preb_attack_flag_;

	UPROPERTY(EditAnywhere, Category = "tp")
		float idle_time_;
	float time_cut_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector noise_pos;

	UFUNCTION(BlueprintCallable, Category = "Set")
		void SetHearPos(FVector pos_);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool is_launch_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool player_can_control_;

	bool is_player_damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* hear_se;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* chase_se;

	UFUNCTION(BlueprintCallable, Category = "Set")
		void SetState(EState ChangeState);

	void PlaySE();
	bool a;
};