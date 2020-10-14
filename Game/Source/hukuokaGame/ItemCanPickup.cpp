//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.cpp
// 概要			：ItemBaseクラスを継承する拾うことが出来るアイテム
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/26
//-------------------------------------------------------------------

#include "ItemCanPickup.h"

AItemCanPickup::AItemCanPickup()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItemCanPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AItemCanPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// プレイヤーにチェックされていたら
	if (m_isChecked)
	{
		// メッシュを白くする
	}
}

// プレイヤーにチェックされたら呼ばれる(作動：状態反転)
void AItemCanPickup::CheckedByPlayer()
{
	// レベル上から自身を消す
	this->Destroy();
}


