//-------------------------------------------------------------------
// ファイル		：CardkeyDoorBody.h
// 概要			：カードキー対応のドアを制御するクラス(修正日:12/11 ItemBaseを継承してコマンドでかざせるように)
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/22
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"
#include "AutomaticDoorLever.h"
#include "CardkeyDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API ACardkeyDoorBody : public AItemBase
{
	// ドアの状態
	enum DOOR_STATE
	{
		kDoorStateLoading = 0,			// カードキー読み込み中
		kDoorStateError,				// 読み取り失敗
		kDoorStateClosed,				// 閉じきっている
		kDoorStateOpened,				// 開ききっている
		kDoorStateClosing,				// 閉じ途中（開→閉）
		kDoorStateOpening,				// 開き途中（閉→開）
	};

	GENERATED_BODY()

public:
	// コンストラクタ
	ACardkeyDoorBody();
	// デストラクタ
	~ACardkeyDoorBody();

protected:
	// 実行時に一度呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UFUNCTION()
		void OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* p_door_body_eventtriggerbox_;	// プレイヤー、敵挟み防止用トリガーボックス

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_cardreader_mesh_1_;	// カード読み込み口のメッシュコンポーネントその1

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_cardreader_mesh_2_;	// カード読み込み口のメッシュコンポーネントその2

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_leftdoor_mesh_;		// 左ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_rightdoor_mesh_;		// 右ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_door_state_mesh_1_;	// 左ドアのロック状態メッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_door_state_mesh_2_;	// 右ドアのロック状態メッシュコンポーネント

	UMaterial* p_door_state_material_;				// ドアのロック状態を表すマテリアル

	UMaterialInstanceDynamic* p_material_instance_dynamic_;

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_open_;			// ドアが開く時のSE

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_close_;		// ドアが閉じる時のSE

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_standby_;		// リーダーの状態色（読み取り可能時）

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_success_;		// リーダーの状態色（読み取り成功時）

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_loading_;		// リーダーの状態色（読み取り中の時）

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_error_;		// リーダーの状態色（読み取り失敗時）

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_loading_success_;	// 読み取り成功時の音

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_loading_error_;		// 読み取り失敗時の音

	APlayerCharacter* p_player_character_;		// プレイヤーキャラクター

	UPROPERTY(EditAnywhere)
		float detect_span_;						// 敵、プレイヤーの検知スパン
	
	UPROPERTY(EditAnywhere)
		float open_and_close_time_;				// 開閉に要する時間

	UPROPERTY(EditAnywhere)
		float check_cardkey_time_;				// カードキー読み取りに掛かる時間

	UPROPERTY(EditAnywhere)
		float display_error_time_;				// カードキー読み取りエラーの表示秒数

	float count_for_check_display_time_;		// カードキー読み取り、エラー表示のカウント用

	float left_door_start_posY_;				// 左ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float left_door_end_posY_;				// 左ドアのY座標終点

	float right_door_start_posY_;				// 右ドアのY座標始点

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 7))
		int door_filter_num_;						// カードとカードリーダーを対応させるための数字

	DOOR_STATE door_state_;						// ドアの状態

	AAutomaticDoorLever* p_alert_lever_;		// 非常レバー(降ろされると強制的に開く)

	bool is_alert_lever_on_;					// 非常レバーが降ろされているか

	UPROPERTY(EditAnywhere)
		bool is_doorbody_eventbox_overlap_;		// ドア本体のイベントボックスにプレイヤー、敵がオーバーラップしているか

	UPROPERTY(EditAnywhere)
		int door_eventbox_overlap_sum_;			// ドア本体のイベントボックス内に入っているプレイヤー、敵の総和(0でis_door_eventbox_overlap = false)

	float open_time_count_;						// 開いている時間のカウント

	float required_time_for_open_and_close_;	// 開閉の所要時間

	float left_door_dire_for_move_;				// 移動するときの向き

	float distance_start_to_end_;				// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	void CheckAlertLeverState();				// 非常レバーの状態チェック

	void UpdateDoorState(float _deltatime);		// ドアの状態更新

	void UpdateDoorMove(float _deltatime);		// ドアの更新

	void CheckDetectSpan(float _deltatime);		// ドアが開ききった後の検知チェック

public:

	// ドアのフィルター番号を返す
	int GetDoorFilter()const { return door_filter_num_; }

	// プレイヤーにチェックされたら呼ばれる(かざす)
	void CheckedByPlayer();
};
