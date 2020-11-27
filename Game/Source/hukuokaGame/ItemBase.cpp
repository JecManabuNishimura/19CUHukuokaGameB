//-------------------------------------------------------------------
// ファイル		：ItemBase.cpp
// 概要			：拾える、調べられる、作動できるActorの基本クラス、
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.h
// 作成者		：19CU0217 朱適
// 更新日		：2020/10/14		白枠の表示の追加
//-------------------------------------------------------------------

#include "ItemBase.h"
// PrimitiveComponentをインクルード		by	朱適
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
AItemBase::AItemBase()
	: m_isChecked(false)
	, m_commandName("")
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

// 白枠の表示の制御		by 朱適
void AItemBase::SetOutline(bool _isCustomDepthOn)
{
	//UActorComponent* meshComponent = GetComponentByClass(UStaticMeshComponent::StaticClass());

	USceneComponent* sceneComponent = GetRootComponent()->GetChildComponent(0);

	if (sceneComponent)
	{
		Cast<UPrimitiveComponent>(sceneComponent)->SetRenderCustomDepth(_isCustomDepthOn);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("cant find outline mesh"));
	}

}
