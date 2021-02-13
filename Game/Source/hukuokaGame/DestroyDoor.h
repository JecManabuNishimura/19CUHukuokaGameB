//-------------------------------------------------------------------
// ファイル		：DestroyDoor.h
// 概要			：赤ちゃんが泣いたとき、敵が突き破るドアの処理の作成
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/12/10
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DestroyDoor.generated.h"

UCLASS()
class HUKUOKAGAME_API ADestroyDoor : public AActor
{
	GENERATED_BODY()

public:
	// コンストラクタ
	ADestroyDoor();

protected:
	// ゲーム開始 or 生成時
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

private:
	float m_time_cnt_;	// タイムカウント
	bool m_impuls_flag_;
	bool m_is_baby_cry;

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pdoor_mesh_;		// 扉本体のメッシュ
	//UPROPERTY(EditAnywhere, Category = "Baby")
	//	bool m_is_baby_cry;								// 終わったらprivateに移動
	UPROPERTY(EditAnywhere, Category = "ImpulseValue")
		FVector m_impulsevalue_;
	UFUNCTION(BlueprintCallable, Category = "Baby")
		void GetIsCry(bool flag) { m_is_baby_cry = flag; }
	UPROPERTY(EditAnywhere, Category = "Mesh")
		float m_disappea_collision_time_;		// 何秒間当たり判定をなくすか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* glass_se_;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* door_se_;
	UPROPERTY(EditAnywhere, Category = "Mesh")
		USceneComponent* root;
};
