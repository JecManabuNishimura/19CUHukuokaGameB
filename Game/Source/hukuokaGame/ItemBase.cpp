//-------------------------------------------------------------------
// ファイル		：ItemBase.cpp
// 概要			：拾える、調べられる、作動できるActorの基本クラス、
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------


#include "ItemBase.h"

// Sets default values
AItemBase::AItemBase()
	: m_isChecked(false)
	, m_commandName("")
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
