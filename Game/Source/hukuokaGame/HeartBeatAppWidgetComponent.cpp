//-------------------------------------------------------------------
// ファイル		：HeartBeatAppWidgetComponent.cpp
// 概要			：心拍数アプリのクラス
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/4
//-------------------------------------------------------------------

#include "HeartBeatAppWidgetComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstance.h"

// インスタンスを初期化
UHeartBeatAppWidgetComponent* UHeartBeatAppWidgetComponent::_instance = NULL;
// コンストラクタ

void UHeartBeatAppWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	_instance = this;
}

// 彩度を設定する関数
void UHeartBeatAppWidgetComponent::_setColorScale(const float _scale)
{
	matInst->ScalarParameterValues[0].ParameterValue = _scale;
	//this->RequestRedraw();
	//Cast<UMeshComponent>(this)->SetScalarParameterValueOnMaterials(TEXT("GrayScale"), _scale);
}

// アプリの起動状態を設定する関数
void UHeartBeatAppWidgetComponent::SetAppStat(bool _bool)
{
	isAppOn = _bool;
	if (isAppOn)
	{
		_setColorScale(grayScale);
		UE_LOG(LogTemp, Log, TEXT("set heart beat off"));
	}
	else
	{
		_setColorScale(colorfulScale);
		UE_LOG(LogTemp, Log, TEXT("set heart beat on"));
	}
}
