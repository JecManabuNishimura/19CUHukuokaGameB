//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.cpp
// 概要			：ItemBaseクラスを継承する自動ドアのロック、解除を管理するレバー
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------


#include "AutomaticDoorLever.h"

AAutomaticDoorLever::AAutomaticDoorLever()
	: m_leverFilter(-1)
	, m_isLeverOn(true)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();
}

void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// プレイヤーにチェックされていたら
	if (m_isChecked)
	{
		// メッシュを白くする
	}
}

// プレイヤーにチェックされたら呼ばれる(作動：状態反転)
void AAutomaticDoorLever::CheckedByPlayer()
{
	// レバーの状態を反転
	m_isLeverOn = !m_isLeverOn;
}

