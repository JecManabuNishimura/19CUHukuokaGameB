//-------------------------------------------------------------------
// ファイル		：GlobalLightController.h
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/18
// 概要			：全てのライトを制御するコントローラー
// 更新履歴		：2020/12/22		ライトのMeshComponentの色変えを追加しました
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalLightController.generated.h"

// 前方宣言
class ULightComponent;

UCLASS()
class HUKUOKAGAME_API AGlobalLightController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalLightController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 全てのライトコンポネントを格納する配列
	TArray<ULightComponent*> lightComponentsArray;

	// 全てのライトのMeshComponentを格納する配列	by	朱適
	TArray<UStaticMeshComponent*> lightMeshCompArray;

public:
	// 全てのライトの色を変更する
	UFUNCTION(BlueprintCallable, Category = "GlobalLightController")
	void SetLightsColor(FLinearColor _color);
};
