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
#include "CardkeyDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API ACardkeyDoorBody : public AItemBase
{
	// ドアの状態
	enum DOOR_STATE
	{
		DOOR_STATE_LOADING = 0,			// カードキー読み込み中
		DOOR_STATE_ERROR,				// 読み取り失敗
		DOOR_STATE_CLOSED,				// 閉じきっている
		DOOR_STATE_OPENED,				// 開ききっている
		DOOR_STATE_CLOSING,				// 閉じ途中（開→閉）
		DOOR_STATE_OPENING,				// 開き途中（閉→開）
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
		UBoxComponent* door_body_eventtriggerbox_;	// プレイヤー、敵挟み防止用トリガーボックス

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* cardreader_mesh_1_;	// カード読み込み口のメッシュコンポーネントその1

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* cardreader_mesh_2_;	// カード読み込み口のメッシュコンポーネントその2

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* leftdoor_mesh_;		// 左ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* rightdoor_mesh_;		// 右ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_state_mesh_1_;	// 左ドアのロック状態メッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_state_mesh_2_;	// 右ドアのロック状態メッシュコンポーネント

	UMaterial* door_state_material;					// ドアのロック状態を表すマテリアル

	UMaterialInstanceDynamic* material_instance_dynamic_;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_open_;			// ドアが開く時のSE

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_close_;			// ドアが閉じる時のSE

	UPROPERTY(EditAnywhere)
		FLinearColor standby_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor success_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor loading_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor error_state_color;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_loading_success;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_loading_error;

	APlayerCharacter* player_character_;

	UPROPERTY(EditAnywhere)
		float m_detectSpan;						// 敵、プレイヤーの検知スパン
	
	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// 開閉に要する時間

	UPROPERTY(EditAnywhere)
		float check_cardkey_time_;				// カードキー読み取りに掛かる時間

	UPROPERTY(EditAnywhere)
		float display_error_time_;				// カードキー読み取りエラーの表示秒数

	float count_for_check_display_time_;		// カードキー読み取り、エラー表示のカウント用

	float m_leftDoorStartPosY;					// 左ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// 左ドアのY座標終点

	float m_rightDoorStartPosY;					// 右ドアのY座標始点

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 7))
		int m_doorFilter;						// カードとカードリーダーを対応させるための数字

	DOOR_STATE m_doorState;						// ドアの状態

	UPROPERTY(EditAnywhere)
		bool is_doorbody_eventbox_overlap;		// ドア本体のイベントボックスにプレイヤー、敵がオーバーラップしているか

	UPROPERTY(EditAnywhere)
		int door_eventbox_overlap_sum;			// ドア本体のイベントボックス内に入っているプレイヤー、敵の総和(0でis_door_eventbox_overlap = false)

	float m_openTimeCount;						// 開いている時間のカウント

	float m_requiredTime;						// 開閉の所要時間

	float m_leftDoorMoveDirection;				// 移動するときの向き

	float m_distanceStartToEnd;					// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	void UpdateDoorState(float _deltatime);		// ドアの状態更新

	void UpdateDoorMove(float _deltatime);		// ドアの更新

	void CheckDetectSpan(float _deltatime);		// ドアが開ききった後の検知チェック

public:

	// ドアのフィルター番号を返す
	int GetDoorFilter()const { return m_doorFilter; }

	// プレイヤーにチェックされたら呼ばれる(かざす)
	void CheckedByPlayer();
};
