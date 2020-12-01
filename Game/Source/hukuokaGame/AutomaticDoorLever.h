//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.h
// 概要			：ItemBaseクラスを継承する自動ドア作動のロック、解除を管理するレバー
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

class AAutomaticDoorBody;

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

private:
	UPROPERTY(EditAnywhere)
		int m_leverFilter;				// レバーとドアを対応させるための数字

	bool m_isLeverOn;					// レバーのON/OFF状態

	bool can_control_;					// 操作可能か

	AAutomaticDoorBody* m_pDoorBody;	// 対応するドア本体のポインタ

public:
	// レバーのON/OFF状態を返す
	UFUNCTION(BlueprintCallable, Category = "Get State")
		bool GetLeverState()const { return m_isLeverOn; }

	// レバーの操作可・不可フラグを返す
	UFUNCTION(BlueprintCallable, Category = "Get State")
		bool GetCanControl()const { return can_control_; }

	// レバーの操作可・不可フラグを設定する
	UFUNCTION(BlueprintCallable, Category = "Set State")
		void SetCanControl(const bool _flag){ can_control_ = _flag; }

	// レバーのON/OFF状態を変更(作成者　尾崎)
	UFUNCTION(BlueprintCallable, Category = "Set State")
		void SetLeverState(const bool flag) { m_isLeverOn = flag; }

	// レバーのフィルター番号を返す
	int GetLeverFilter()const { return m_leverFilter; }
};