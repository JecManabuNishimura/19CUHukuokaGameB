//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.h
// 概要			：ItemBaseクラスを継承する自動ドアのロック、解除を管理するレバー
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorLever : public AItemBase
{
	GENERATED_BODY()
	
public:	
	AAutomaticDoorLever();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// プレイヤーにチェックされたら呼ばれる(作動)
	virtual void CheckedByPlayer();

public:
	// レバーのON/OFF状態を返す
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnLeverState(){ return m_isLeverOn; }

	UPROPERTY(EditAnywhere)
		int m_leverFilter;					// レバーとドアを対応させるための数字

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;					// レバーのON/OFF状態
};
