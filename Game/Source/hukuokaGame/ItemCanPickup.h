//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.h
// 概要			：ItemBaseクラスを継承する拾うことが出来るアイテム
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "ItemBase.h"
#include "ItemCanPickup.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemCanPickup : public AItemBase
{
	GENERATED_BODY()

public:
	AItemCanPickup();

	virtual ~AItemCanPickup() {}

protected:
	virtual void BeginPlay() override;

private:
	APlayerCharacter* player_character_;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 7))
		int cardkey_filter_;

public:
	virtual void Tick(float DeltaTime) override;

	// プレイヤーにチェックされたら呼ばれる(作動)
	virtual void CheckedByPlayer();
};
