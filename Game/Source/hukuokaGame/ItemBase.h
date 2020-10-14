//-------------------------------------------------------------------
// ファイル		：ItemBase.h
// 概要			：拾える、調べられる、作動できるActorの基本クラス(Map上のアイテム)
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：ItemCanPickup.h
// 作成者		：19CU0217 朱適
// 更新日		：2020/10/14		白枠の表示の追加
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// プレイヤーのチェックを受ける
	virtual void CheckedByPlayer() {}

public:
	// プレイヤーによる被チェック状態を返す
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnCheckedState() { return m_isChecked; }
public:
	bool m_isChecked;			// プレイヤーにチェックされているか

	UPROPERTY(EditAnywhere)
	FString m_commandName;		// コマンドの名前(種類)

	void SetOutline(bool _isCustomDepthOn);
};
