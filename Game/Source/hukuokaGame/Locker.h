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
		int max_rotation;					// 何度まで開くか
	UPROPERTY(EditAnywhere)
		int open_and_close_frame;			// 開く・閉じるときにかかるフレーム数
	UPROPERTY(EditAnywhere)
		int player_change_rotation_frame;	// ロッカーが閉まった時にプレイヤーの視点が変わる時のフレーム数
	UPROPERTY(EditAnywhere)
		int player_to_locker_frame;			// プレイヤーがロッカーに入る・出る時のフレーム数

	// 関数
		// プレイヤーにチェックされたら呼ばれる(作動)
	virtual void CheckedByPlayer();

	void GetPlayer();

private:
	APlayerCharacter* player;

	bool is_end_rotation_;		// 回転をし終えたかどうか
	bool can_input_;			// 入力可能かどうか
	bool is_check_;				// プレイヤーが調べたか
	bool is_move_in_locker_;		// ロッカーに移動中かどうか
	bool is_in_player_;			// 開く->閉じるの時にtrueにする
	bool player_rotation_start_flag_;	// プレイヤーの回転を開始させるかどうかのフラグ
	float add_rotation_value_;	// 1フレームごとにどれだけ扉を回転させるか
	float now_rotation_value_;	// 現在の角度 
	float add_player_rotation_value_; // 1フレームごとにどれだけプレイヤーを回転させるか
	FVector locker_body_location_;	// ロッカーの本体の位置
	FVector player_location_save_;	// ロッカーに入ろうとしたときのPlayerの座標格納用(ロッカーから出る時に必要)
	FVector player_move_vector_;		// 1フレームごとにプレイヤーがどれだけ進むかを格納
	FRotator locker_body_rotation_;	// ロッカー本体の回転
	float location_lerp_alpha;
	float location_add_lerp_value;
	float rotation_lerp_alpha;
	float rotation_add_lerp_value;
	float def_door_rotation;

	void SetDoorRotationValue();
	void UpdateDoorRotation();
	void InToLocker();
	void OutToLocker();
	void CloseDoor();
	void PlayerRotation();
};