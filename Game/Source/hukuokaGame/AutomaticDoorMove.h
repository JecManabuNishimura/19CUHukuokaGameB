//-------------------------------------------------------------------
// ファイル		：AutomaticDoorMove.h
// 概要			：自動ドアを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/20
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"				// UBoxComponentを使うために必要なヘッダ
#include "Components/StaticMeshComponent.h"
#include "AutomaticDoorLever.h"
#include "AutomaticDoorMove.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorMove : public AActor
{
	GENERATED_BODY()
	
public:
	// コンストラクタ
	AAutomaticDoorMove();
	// デストラクタ
	~AAutomaticDoorMove();

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

	bool m_isSwitchOn;							// 自動ドアを作動させるためのレバーフラグ

	bool m_canMove;								// ドアの開閉処理を行うかのフラグ

	bool m_isOpened;							// ドアが開ききっているかのフラグ

	bool m_isOpening;							// ドアが開き途中かのフラグ

	bool m_isOverlap;							// プレイヤーもしくは敵が検知内に入っているかのフラグ

	float m_openTimeCount;						// 開いている時間のカウント

	float m_leftDoorMoveDirection;				// 移動するときの向き

	float m_distanceStartToEnd;					// ドアの始点から終点までの距離(片方で取得し、もう片方でもそれを利用)

	void CheckDetectSpan(float _deltaTime);		// ドアが開ききった後の検知チェック

	void UpdateDoorMove(float _deltaTime);		// ドアの更新
};
