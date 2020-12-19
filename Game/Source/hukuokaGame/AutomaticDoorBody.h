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
		DOOR_STATE_CLOSED = 0,			// 閉じきっている
		DOOR_STATE_OPENED,				// 開ききっている
		DOOR_STATE_CLOSING,				// 閉じ途中（開→閉）
		DOOR_STATE_OPENING,				// 開き途中（閉→開）
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
		UBoxComponent* m_pEventTriggerBox;		// プレイヤー、敵検知用トリガーボックス

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pLeftDoorComp;	// 左ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRightDoorComp;	// 右ドアのメッシュコンポーネント

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* lamp_for_decide_pos_;	// 生成するランプの位置を決めるためのランプメッシュ

	UPROPERTY(EditAnywhere)
		UStaticMesh* lever_state_lamp_;			// レバーの状態を表すランプメッシュ

	UPROPERTY(EditAnywhere)
		UMaterial* lever_on_state_material_;

	UPROPERTY(EditAnywhere)
		UMaterial* lever_off_state_material_;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_open_;			// ドアが開く時のSE

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_close_;			// ドアが閉じる時のSE

	UPROPERTY(EditAnywhere)
		float m_detectSpan;						// 敵、プレイヤーの検知スパン

	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// 開閉に要する時間

	float m_leftDoorStartPosY;					// 左ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// 左ドアのY座標終点

	float m_rightDoorStartPosY;					// 右ドアのY座標始点

	UPROPERTY(EditAnywhere)
		int m_doorFilter;						// レバーとドアを対応させるための数字

	FVector lamp_generate_pos_;					// ランプを生成する場所

	bool m_isSwitchOn;							// 自動ドアを作動させるためのフラグ

	bool m_isOverlap;							// プレイヤーもしくは敵が検知内に入っているかのフラグ

	float m_openTimeCount;						// 開いている時間のカウント

	float m_requiredTime;						// 開閉の所要時間

	float m_leftDoorMoveDirection;				// 移動するときの向き

	float m_distanceStartToEnd;					// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	int m_detectNum;							// 検知範囲内いるプレイヤー、敵の総和

	DOOR_STATE m_doorState;						// ドアの状態

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
	int GetDoorFilter()const { return m_doorFilter; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int items_Mission_Num;					// 持っているミッションナンバー、ないなら0 (作成者:林雲暉)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int next_Items_Mission_Num;				// 次のミッションナンバー、ないなら0		(作成者:林雲暉)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool isMissionComplete;					// 対応したミッション完成しましたか			(作成者:林雲暉)

};
