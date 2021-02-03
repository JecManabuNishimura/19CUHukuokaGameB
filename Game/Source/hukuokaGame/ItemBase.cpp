//-------------------------------------------------------------------
// ファイル		：ItemBase.cpp
// 概要			：拾える、調べられる、作動できるActorの基本クラス、
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：ItemBase.cpp
// 作成者		：19CU0217 朱適
// 更新日		：2020/10/14		白枠の表示の追加
// 更新日		：2020/12/12		SetOutlineのオブジェクトを指定できるようにする
//-------------------------------------------------------------------

#include "ItemBase.h"
// PrimitiveComponentをインクルード		by	朱適
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
AItemBase::AItemBase()
	: sound_when_checked_(NULL)
	, command_name_("")
	, is_checked_(false)
	, items_Mission_Num(0)
	, isMissionComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 白枠の表示の制御	by 朱適 (12/13 修正者:増井 引数にコンポーネントのインデックスを追加)
void AItemBase::SetOutline(bool _isCustomDepthOn, const int _checking_comp_index)
{
	USceneComponent* sceneComponent = GetRootComponent()->GetChildComponent(_checking_comp_index);

	if (sceneComponent)
	{
		Cast<UPrimitiveComponent>(sceneComponent)->SetRenderCustomDepth(_isCustomDepthOn);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("cant find outline mesh"));
	}
}
