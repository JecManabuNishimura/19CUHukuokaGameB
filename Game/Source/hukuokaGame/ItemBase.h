//-------------------------------------------------------------------
// ファイル		：ItemBase.h
// 概要			：拾える、調べられる、作動できるActorの基本クラス(Map上のアイテム)
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：ItemBase.h
// 作成者		：19CU0217 朱適
// 更新日		：2020/10/14		白枠の表示の追加
// 更新日		：2020/12/12		SetOutlineのオブジェクトを指定できるようにする
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "ItemBase.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();

	virtual ~AItemBase() {}

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// プレイヤーのチェックを受ける
	virtual void CheckedByPlayer() {}

protected:
	UPROPERTY(EditAnywhere)
		USoundBase* sound_when_checked_;	// チェックされた時の効果音

	UPROPERTY(EditAnywhere)
		FString command_name_;				// コマンドの名前(種類)

	UPROPERTY(EditAnywhere)
		bool can_checked_when_player_chased_;	// プレイヤーが敵に追われているときにチェックできるアイテムか

protected:
	bool is_checked_;						// プレイヤーにチェックされているか

public:
	// プレイヤーによる被チェック状態を設定
	void SetCheckedState(const bool _new_state) { is_checked_ = _new_state; }

	// プレイヤーが敵に追われているときにチェック出来るアイテムかどうかを返す
	bool GetCanCheckedWhenPlayerChased()const { return can_checked_when_player_chased_; }

	// コマンド名を返す
	FString GetCommandName()const { return command_name_; }

public:
	// プレイヤーによる被チェック状態を返す
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnCheckedState()const { return is_checked_; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int items_Mission_Num;					// 持っているミッションナンバー、ないなら0	(作成者:林雲暉)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool isMissionComplete;					// 対応したミッション完成しましたか			(作成者:林雲暉)

public:
	void SetOutline(bool _isCustomDepthOn, const int _checking_comp_index);
};
