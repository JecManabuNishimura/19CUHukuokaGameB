//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.cpp
// 概要			：ItemBaseクラスを継承する拾うことが出来るアイテム
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/26
//-------------------------------------------------------------------

#include "ItemCanPickup.h"

AItemCanPickup::AItemCanPickup()
	: player_character_(NULL)
	, cardkey_filter_(0)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItemCanPickup::BeginPlay()
{
	Super::BeginPlay();
	
	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AItemCanPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// プレイヤーにチェックされたら呼ばれる(作動：状態反転)
void AItemCanPickup::CheckedByPlayer()
{
	// 対応するフラグを立てる
	player_character_->SetHaveCardkeyState(cardkey_filter_);

	// 取得音を鳴らす
	if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

	// ミッションに反映する場合、スマホのミッションをアップデート
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
		} // end if()

	} // end if()

	// レベル上から自身を消す
	this->Destroy();
}