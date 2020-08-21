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


private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEventTriggerBox;	// プレイヤー、敵検知用トリガーボックス

	UPROPERTY(EditAnywhere)
		float m_detectSpan;					// 敵、プレイヤーの検知スパン

	UPROPERTY(EditAnywhere)
		float m_doorStartPosY;				// ドアのY座標始点

	UPROPERTY(EditAnywhere)
		float m_doorEndPosY;				// ドアのY座標終点

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;					// 自動ドアを作動させるためのレバーフラグ

	float m_openTimeCount;					// 開いている時間のカウント

};
