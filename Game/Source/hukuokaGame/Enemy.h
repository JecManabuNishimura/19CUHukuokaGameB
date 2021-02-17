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

// 前方宣言
class AAIController;

// 敵のステートの宣言
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

	TArray<FVector> targetpoint_pos_;		// 対応しているtargetpointの位置情報取得用
	FVector last_see_pos;					// 最後に視認した座標

	void CheckMoveToTargetPoint();			// 移動先に到着したか確認する関数
	void IdleCoolDown(float _deltatime);	// Idle状態の解除をするか確認する関数
	void Patrol(FVector _pos);		// Patrol状態の時に呼ばれる関数
	void Pursue_Chase();			// Chase状態の時に呼ばれる関数
	void Pursue_Hear();				// Hear状態の時に呼ばれる関数
	void OutSeePlayer();			// Chase状態で、Playerが視野の外にいる場合に呼ばれる関数
	void PlaySE();					// SEを鳴らす関数
	void LoseSight_Chase();			// 見失った時
	void CheckMoveToLastSeePos();	// 見失った地点まで行ったかどうかを確認する関数


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool attack_flag_;							// 攻撃終了を検知するためのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool is_launch_;							// ゲーム開始時に動かして良いかどうかのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "tp")
		bool player_can_control_;					// プレイヤーの操作可能かどうかのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EState enemy_state_;						// 敵の状態格納用

	UPROPERTY(EditAnywhere, Category = "tp")
		float headLine_;							// 頭の高さの設定用(レイを飛ばす処理にて使用)

	UPROPERTY(EditAnywhere, Category = "tp")
		float idle_time_;							// 待機時間

	UPROPERTY(EditAnywhere, Category = "tp")
		float chase_end_length_;					// 追跡終了をさせる距離

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector noise_pos_;							// 音を発した地点

	UPROPERTY(VisibleAnywhere, Category = "pawnsensing")
		class UPawnSensingComponent* ppawnsensing_;	// pawnsensing格納用

	UPROPERTY(EditAnywhere, Category = "tp")
		TArray<ATargetPoint*> ptargetpoint_;		// targetpoint格納用

			// 攻撃の当たり判定
	UPROPERTY(EditAnywhere, Category = "tp")
		UBoxComponent* attack_collision_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* hear_se_;						// 音を検知した際に鳴らすSE格納用

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* chase_se_;						// 視覚で検知した際に鳴らすSE格納用

// ----------------------------------------------------------------------------------------

	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);											// プレイヤーを視認した際に呼ばれる

	UFUNCTION()
		void OnHear(APawn* OtherActor, const FVector& Location, float Volume);	// 実装の目処が立ってない

	// attack_collision_にて使用
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Set")								// 外部オブジェクトにて音を発した地点を格納する関数
		void SetHearPos(FVector _pos);

	UFUNCTION(BlueprintCallable, Category = "Set")
		void SetState(EState _changeState);

// ----------------------------------------------------------------------------------------

	bool in_eye_;				// 視野の中にいるかどうか
	bool chase_flag_;			// 視認した際にたてるフラグ
	bool is_player_damage_;		// ダメージを受ける状態か否かを確認する関数(これが無いとtick関数で何度も呼ばれてしまうため作成)
	FHitResult hitresult_;		// レイキャストの結果
	float time_cut_;			// Idle状態の経過時間格納用
	int tp_index_;				// targetpointの要素数

	AEnemyMyAIController* AIController;		// コントローラー取得
	APlayerCharacter* Player;				// player取得
};