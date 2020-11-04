//-------------------------------------------------------------------
// ファイル		：HeartBeatAppWidgetComponent.h
// 概要			：心拍数アプリのクラス
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/4
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HeartBeatAppWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class HUKUOKAGAME_API UHeartBeatAppWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	// インスタンスを取得する関数
	static UHeartBeatAppWidgetComponent* GetInstance() { return _instance; }
	// begin play
	virtual void BeginPlay() override;

private:
	// heart beat app is on/off
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		bool isAppOn;
	// heart beat speed
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float beatSpeed;
	// beat scale
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float scaleSize;
	// beat frequency
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float beatFrequency;
	//UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		UMaterialInstance* matInst;

	// 色のスケール
	// 起動していない時
	const float grayScale = 0.5f;
	// 起動している時
	const float colorfulScale = 1.0f;

	// インスタンス
	static UHeartBeatAppWidgetComponent* _instance;

private:
	// 彩度を設定する関数
	void _setColorScale(const float _scale);
public:
	// アプリの起動状態を設定する関数
	void SetAppStat(bool _bool);

};
