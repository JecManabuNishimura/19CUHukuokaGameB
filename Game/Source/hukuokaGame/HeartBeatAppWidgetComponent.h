//-------------------------------------------------------------------
// ファイル		：HeartBeatAppWidgetComponent.h
// 概要			：心拍数アプリのクラス
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/4
// 更新日		：2020/11/6 isAppOnフラグをブループリントにアクセスできるようにする
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HeartBeatAppWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "UserInterface", hidecategories = (Object, Activation, "Components|Activation", Sockets, Base, Lighting, LOD, Mesh), editinlinenew, meta = (BlueprintSpawnableComponent))
class HUKUOKAGAME_API UHeartBeatAppWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	// インスタンスを取得する関数
	static UHeartBeatAppWidgetComponent* GetInstance() { return _instance; }
	// begin play
	virtual void BeginPlay() override;

public:
	// アプリの起動状態を設定する関数
	void SetAppStat(bool _bool);

	// isAppOnを取得する関数（作成者：朱適）
	UFUNCTION(BlueprintGetter)
	bool GetIsAppOn() { return isAppOn; }

	// isAppOnを設定する関数（作成者：朱適）
	UFUNCTION(BlueprintSetter)
	void SetIsAppOn(bool _bool) { isAppOn = _bool; }


	// isScaleingを取得する関数（作成者：朱適）
	UFUNCTION(BlueprintGetter)
	bool GetIsScaling() { return isScaling; }

	// isScaleingを設定する関数（作成者：朱適）
	UFUNCTION(BlueprintSetter)
	void SetIsScaling(bool _bool) { isScaling = _bool; }

	// scaleSizeを取得する関数（作成者：朱適）
	UFUNCTION(BlueprintGetter)
	float GetScaleSize() { return scaleSize; }

	// beatDelayを取得する関数（作成者：朱適）
	UFUNCTION(BlueprintGetter)
	float GetBeatDelay() { return beatDelay; }

	// beatDelayを設定するする関数（作成者：朱適）
	UFUNCTION(BlueprintSetter)
	void SetBeatDelay(float _value) { beatDelay = _value; }

private:
	// ブループリントに取得できるようにする　（作成者：朱適）
	// heart beat app is on/off
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsAppOn, BlueprintGetter = GetIsAppOn, Category = "AppWidget Parameters")
		bool isAppOn;
	// heart is Scaleing or not
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsScaling, BlueprintGetter = GetIsScaling, Category = "AppWidget Parameters")
		bool isScaling;
	// beat scale
	UPROPERTY(EditAnywhere, BlueprintGetter = GetScaleSize, Category = "AppWidget Parameters")
		float scaleSize;
	// beat delay
	UPROPERTY(EditAnywhere, BlueprintSetter = SetBeatDelay, BlueprintGetter = GetBeatDelay, Category = "AppWidget Parameters")
		float beatDelay;
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


};
