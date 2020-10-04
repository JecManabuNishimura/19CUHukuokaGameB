//-------------------------------------------------------------------
// ファイル		：AutomaticDoorBody.h
// 概要			：自動ドア本体を制御するクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/04
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutomaticDoorBody.generated.h"

// 前方宣言
class UBoxComponent;
class UStaticMeshComponent;
class AAutomaticDoorLever;

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
		float m_detectSpan;						// 敵、プレイヤーの検知スパン

	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// 開閉に要する時間

	float m_leftDoorStartPosY;					// 左ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// 左ドアのY座標終点

	float m_rightDoorStartPosY;					// 右ドアのY座標始点

	UPROPERTY(EditAnywhere)
		int m_doorFilter;						// レバーとドアを対応させるための数字

	bool m_isSwitchOn;							// 自動ドアを作動させるためのフラグ

	bool m_isOverlap;							// プレイヤーもしくは敵が検知内に入っているかのフラグ

	float m_openTimeCount;						// 開いている時間のカウント

	float m_requiredTime;						// 開閉の所要時間

	float m_leftDoorMoveDirection;				// 移動するときの向き

	float m_distanceStartToEnd;					// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	int m_detectNum;							// 検知範囲内いるプレイヤー、敵の総和

	DOOR_STATE m_doorState;						// ドアの状態

	TArray<AAutomaticDoorLever*> m_filterMatchLevers;	// (対応するレバー)m_doorFilterを保存する配列

	void UpdateDoorState();						// ドアの状態更新

	void UpdateDoorMove(float _deltaTime);		// ドアの更新

	void CheckDetectSpan(float _deltaTime);		// ドアが開ききった後の検知チェック

public:
	// ドア本体のスイッチの更新
	void UpdateSwitchState(const bool _isLeverOn = true);

	// ドアのフィルター番号を返す
	int GetDoorFilter()const { return m_doorFilter; }
};
