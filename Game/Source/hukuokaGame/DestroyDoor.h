//-------------------------------------------------------------------
// ファイル		：DestroyDoor.h
// 概要			：赤ちゃんが泣いたとき、敵が突き破るドアの処理の作成
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/12/
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
	//bool m_is_baby_cry;

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pdoor_mesh_;		// 扉本体のメッシュ
	//UPROPERTY(EditAnywhere, Category = "Mesh")
		//UStaticMeshComponent* m_pdoorglass_mesh;	// 扉に付いているガラスのメッシュ(余力があれば倒れるときに割ることを出来るようにするため)
	UPROPERTY(EditAnywhere, Category = "Baby")
		bool m_is_baby_cry;								// 終わったらprivateに移動
	UPROPERTY(EditAnywhere, Category = "ImpulseValue")
		FVector m_Impulsevalue;
	UFUNCTION(BlueprintCallable, Category = "Baby")
		void GetIsCry(bool flag) { m_is_baby_cry = flag; }
};
