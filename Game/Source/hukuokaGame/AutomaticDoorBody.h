//-------------------------------------------------------------------
// ファイル		：AutomaticDoorBody.h
// 概要			：自動ドア本体を制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/04
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorBody : public AActor
{
	// ドアの状態
	enum DOOR_STATE
	{
		kDoorStateClosed = 0,			// 閉じきっている
		kDoorStateOpened,				// 開ききっている
		kDoorStateClosing,				// 閉じ途中（開→閉）
		kDoorStateOpening,				// 開き途中（閉→開）
	};

	GENERATED_BODY()

public:
	// コンストラクタ
	AAutomaticDoorBody();
	// デストラクタ
	~AAutomaticDoorBody();

protected:
	// 実行時に一度呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	UPROPERTY(EditAnywhere)
		UBoxComponent* p_eventTriggerBox_;		// プレイヤー、敵検知用トリガーボックス

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_left_door_comp_;	// 左ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_right_door_comp_;	// 右ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_lamp_for_decide_pos_;	// 生成するランプの位置を決めるためのランプメッシュ

	UPROPERTY(EditAnywhere)
		UStaticMesh* p_lever_state_lamp_;			// レバーの状態を表すランプメッシュ

	UPROPERTY(EditAnywhere)
		UMaterial* p_lever_on_state_material_;	// レバーがオンの時のマテリアル

	UPROPERTY(EditAnywhere)
		UMaterial* p_lever_off_state_material_;	// レバーがオフの時のマテリアル

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_open_;			// ドアが開く時のSE

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_close_;		// ドアが閉じる時のSE

	UPROPERTY(EditAnywhere)
		float detect_span_;						// 敵、プレイヤーの検知スパン

	UPROPERTY(EditAnywhere)
		float open_and_close_time_;				// 開閉に要する時間

	float left_door_start_posY_;				// 左ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float left_door_end_posY_;				// 左ドアのY座標終点

	float right_door_start_posY_;				// 右ドアのY座標始点

	UPROPERTY(EditAnywhere)
		int door_filter_num_;					// レバーとドアを対応させるための数字

	FVector lamp_generate_pos_;					// ランプを生成する場所

	bool is_switch_on_;							// 自動ドアを作動させるためのフラグ

	bool is_overlap_;							// プレイヤーもしくは敵が検知内に入っているかのフラグ

	float open_time_count_;						// 開いている時間のカウント

	float required_time_for_open_and_close_;	// 開閉の所要時間

	float left_door_dire_for_move_;				// 移動するときの向き

	float distance_start_to_end_;				// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	int detect_num_;							// 検知範囲内いるプレイヤー、敵の総和

	DOOR_STATE door_state_;						// ドアの状態

	TArray<AAutomaticDoorLever*> filter_match_levers_;	// 対応するレバーを保存する配列

	int filter_match_levers_num_;				// 対応するレバーの数

	TArray<UStaticMeshComponent*> match_lever_state_lamps_;	// 対応するレバーの状態を表すランプメッシュ

	void UpdateDoorState();						// ドアの状態更新

	void UpdateDoorMove(float _deltaTime);		// ドアの更新

	void CheckDetectSpan(float _deltaTime);		// ドアが開ききった後の検知チェック

	void GoToPlayerCharacterAndUpdateMission();	// プレイヤーのUpdateTheMissionを呼び出すためのメゾット (作成者:林雲暉)

public:
	// ドア本体のスイッチの更新
	void UpdateSwitchState(const AAutomaticDoorLever* const operated_lever = nullptr);

	// ドアのフィルター番号を返す
	int GetDoorFilter()const { return door_filter_num_; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int items_Mission_Num;					// 持っているミッションナンバー、ないなら0 (作成者:林雲暉)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int next_Items_Mission_Num;				// 次のミッションナンバー、ないなら0		(作成者:林雲暉)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool isMissionComplete;					// 対応したミッション完成しましたか			(作成者:林雲暉)

};
