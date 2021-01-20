//-------------------------------------------------------------------
// ファイル		：Locker.h
// 概要			：ロッカーの制御
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/11/16
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Locker.generated.h"

UCLASS()
class HUKUOKAGAME_API ALocker : public AItemBase
{
	GENERATED_BODY()
	
public:

	// コンストラクタ
	ALocker();

	// デストラクタ
	~ALocker();

protected:
	// 実行時に一度呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

public:
	// メッシュ関係
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* body_mesh_;		// 本体のメッシュ
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_mesh_;		// 扉のメッシュ

	// 値
	UPROPERTY(EditAnywhere)
		int max_rotation_;					// 何度まで開くか
	UPROPERTY(EditAnywhere)
		float open_and_close_second_;			// 開く・閉じるときにかかる秒数
	UPROPERTY(EditAnywhere)
		float player_change_rotation_second_;	// ロッカーが閉まった時にプレイヤーの視点が変わる時の秒数
	UPROPERTY(EditAnywhere)
		float player_to_locker_second_;			// プレイヤーがロッカーに入る・出る時の秒数
	UPROPERTY(EditAnywhere)
		int flont_distance_;					// プレイヤーがロッカーに入る・出る時の秒数
	UPROPERTY(EditAnywhere)
		USoundBase* close_se_;	// チェックされた時の効果音

	// 関数
		// プレイヤーにチェックされたら呼ばれる(作動)
	virtual void CheckedByPlayer();

	void GetPlayer();

private:
	APlayerCharacter* player;

	bool is_end_rotation_;				// 回転をし終えたかどうか
	bool can_input_;					// 入力可能かどうか
	bool is_check_;						// プレイヤーが調べたか
	bool is_move_in_locker_;			// ロッカーに移動中かどうか
	bool is_in_player_;					// 開く->閉じるの時にtrueにする
	bool player_rotation_start_flag_;	// プレイヤーの回転を開始させるかどうかのフラグ
	float add_rotation_value_;			// 1フレームごとにどれだけ扉を回転させるか
	float now_rotation_value_;			// 現在の角度 
	float add_player_rotation_value_;	// 1フレームごとにどれだけプレイヤーを回転させるか
	FVector locker_body_location_;		// ロッカーの本体の位置
	FVector player_location_save_;		// ロッカーに入ろうとしたときのPlayerの座標格納用(ロッカーから出る時に必要)
	FVector player_move_vector_;		// 1フレームごとにプレイヤーがどれだけ進むかを格納
	FVector body_front_location_;		// ロッカーの前の場所(Playerが出るときなどに特定の地点に移動させるため)
	FRotator locker_body_rotation_;		// ロッカー本体の回転
	float location_lerp_alpha1_;		// player_location_save_ -> body_front_location_ の移動で使う
	float location_lerp_alpha2_;		// body_front_location_  -> locker_body_location_　の移動で使う
	float location_add_lerp_value_;		// 毎フレームどれだけlerpのalphaを変えるか(移動用)
	float rotation_lerp_alpha_;			// Playerの回転
	float rotation_add_lerp_value_;		// 毎フレームどれだけlerpのalphaを変えるか(回転用)
	float def_door_rotation_;			// 起動時のドアの角度の格納

	void SetDoorRotationValue(float DeltaTime);		// ドアの回転に関する値の設定
	void UpdateDoorRotation(float DeltaTime);		// ドアの回転
	void InToLocker(float DeltaTime);				// ロッカーの中に入る処理
	void OutToLocker(float DeltaTime);				// ロッカーから出る処理
	void PlayerRotation(float DeltaTime);			// Playerの角度の変更
};