//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.cpp
// 概要			：ItemBaseクラスを継承する拾うことが出来るアイテム
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/26
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.cpp
// 作成者		：19CU0236 林雲暉 
// 更新日		：2021/03/07			アイテムヒントを追加
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

	// アイテムヒントを生成する (作成者:林雲暉)
	bp_ItemInfo = TSoftClassPtr<AActor>(FSoftObjectPath("Blueprint'/Game/Blueprints/BP_ItemHint3D.BP_ItemHint3D_C'")).LoadSynchronous();	// pathにあるクラスを取得
	if (bp_ItemInfo != nullptr && isNeedToDiaplsy == true)
	{
		itemInfoActor = GetWorld()->SpawnActor<AActor>(bp_ItemInfo);						// アイテムヒントをActorとして生成する

		if (itemInfoActor != NULL)
		{

			itemInfoActor->SetActorEnableCollision(false);

			itemInfoActor->SetActorLocation((this->GetActorLocation() + infoPosition));

			FOutputDeviceNull ar;
			FString FuncName_and_Solution1 = FString::Printf(TEXT("InitialHeight "));
			FString FuncName_and_Solution2 = FString::Printf(TEXT("InitialScale "));
			FString FuncName_and_Solution3 = FString::Printf(TEXT("InitialDistance "));

			FuncName_and_Solution1 += FString::SanitizeFloat(infoWorkingHeight);
			FuncName_and_Solution2 += FString::SanitizeFloat(infoScale);
			FuncName_and_Solution3 += FString::SanitizeFloat(toPlayers_MinDistance);

			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution1, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution2, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution3, ar, NULL, true);

		} // end if()
		else {
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist"));
	} // end else
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

	// ミッションに反映する場合、スマホのミッションをアップデート  (作成者:林雲暉)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
		} // end if()

	} // end if()

	// 拾ったらアイテムヒントを消す　(作成者:林雲暉)
	itemInfoActor->Destroy();

	// レベル上から自身を消す
	this->Destroy();
}