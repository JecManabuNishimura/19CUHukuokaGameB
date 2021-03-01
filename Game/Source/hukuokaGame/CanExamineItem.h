//-------------------------------------------------------------------
// ファイル		：CanExamineItem.h
// 概要			：ItemBaseクラスを継承するファイル等の調べられるアイテムクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CanExamineItem.generated.h"

UCLASS()
class HUKUOKAGAME_API ACanExamineItem : public AItemBase
{
	GENERATED_BODY()

public:
	ACanExamineItem();

	virtual ~ACanExamineItem() {}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_photo_mesh_;	// 写真のメッシュ

	UPROPERTY(EditAnywhere)
		float distance_file_to_player_;	// 調べたときのプレイヤーからファイルまでの距離
private:
	// プレイヤー
	APlayerCharacter* p_playercharacter_;

	FTransform transform_on_map_;		// マップ上に配置されている時の情報(調べ終えたときに利用)

	bool is_show_details_;				// 現在調べられているか

	bool do_file_loc_correction_;		// ファイル位置補正用フラグ

public:
	virtual void Tick(float DeltaTime) override;

	// プレイヤーにチェックされたら呼ばれる(調べる)
	void CheckedByPlayer();

};