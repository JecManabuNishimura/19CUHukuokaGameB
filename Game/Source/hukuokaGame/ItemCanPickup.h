//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.h
// 概要			：ItemBaseクラスを継承する拾うことが出来るアイテム
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemCanPickup.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemCanPickup : public AItemBase
{
	GENERATED_BODY()

public:
	AItemCanPickup();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// プレイヤーにチェックされたら呼ばれる(作動)
	virtual void CheckedByPlayer();

public:
};
