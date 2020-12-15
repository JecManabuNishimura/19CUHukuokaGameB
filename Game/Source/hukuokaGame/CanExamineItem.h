//-------------------------------------------------------------------
// ファイル		：CanExamineItem.h
// 概要			：ItemBaseクラスを継承するファイル等の調べられるアイテムクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "CanExamineItem.generated.h"

// イベントディスパッチャー宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFileTrunPageEventDispatcher);		// プレイヤーの視線がCanCheckアイテムに当たった時

// ファイルの構造体(string型10個)
USTRUCT(BlueprintType)
struct FFileTextStruct: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FFileTextStruct() {}

public:
	UPROPERTY(EditAnywhere)
		FString page1;

	UPROPERTY(EditAnywhere)
		FString page2;

	UPROPERTY(EditAnywhere)
		FString page3;

	UPROPERTY(EditAnywhere)
		FString page4;

	UPROPERTY(EditAnywhere)
		FString page5;

	UPROPERTY(EditAnywhere)
		FString page6;

	UPROPERTY(EditAnywhere)
		FString page7;

	UPROPERTY(EditAnywhere)
		FString page8;

	UPROPERTY(EditAnywhere)
		FString page9;

	UPROPERTY(EditAnywhere)
		FString page10;
};

UCLASS()
class HUKUOKAGAME_API ACanExamineItem : public AItemBase
{
	GENERATED_BODY()

public:
	ACanExamineItem();

protected:
	virtual void BeginPlay() override;

private:
	// プレイヤー
	APlayerCharacter* player_character_;

	// ファイルのスケルタルメッシュ
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* file_mesh_;

	UPROPERTY(EditAnyWhere)
		UDataTable* datatable_;			// ファイルのテキストデータ

	FTransform transform_on_map_;		// マップ上に配置されている時の情報(調べ終えたときに利用)

	FLinearColor text_linear_color_;	// テキストのカラー

	TArray<FString> text_in_file_kind_;	// 指定したファイル番号の全テキストデータ格納用変数 

	FString left_text_;					// 左ページに表示する文字

	FString right_text_;				// 右ページに表示する文字

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int file_kind_;					// ファイルの識別番号

	int page_num_;						// 総ページ数

	int left_page_open_now_num_;		// 表示中の左ページ数

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_open_close_;			// 調べてからアニメーションし始めるまでの秒数

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_display_text_;		// 調べてからテキストがフェードインし始めるまでの秒数

	UPROPERTY(EditAnywhere)
		float distance_from_file_to_player_;	// 調べたときのプレイヤーからファイルまでの距離

	float count_for_time_open_close_;	// アニメーション、テキストフェードイン管理用のカウント

	bool is_show_details_;				// プレイヤーにチェックされているかのフラグ

	bool can_start_anim_;				// アニメーション開始許可のフラグ

	bool can_turn_page_;				// ページをめくる許可フラグ

	bool can_show_detial_;				// 戻そうとするときに再度調べてしまうのを防止するためのフラグ

	bool do_file_loc_correction_;		// ファイル位置補正用フラグ

	void TurnPage();

	UPROPERTY(BlueprintAssignable)
		FOnFileTrunPageEventDispatcher  OnFileTrunPageEventDispatcher;
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		UWidgetComponent* widget_comp_;	// エディタ上で生成したものを取得

	// プレイヤーにチェックされたら呼ばれる(作動)
	void CheckedByPlayer();

	// ファイルの識別番号を返す
	UFUNCTION(BlueprintCallable)
		int ReturnFileKind() { return file_kind_; }

	// アニメーション開始許可フラグを返す
	UFUNCTION(BlueprintCallable)
		bool ReturnCanStartAnim() { return can_start_anim_; }

	// テキストの色を返す
	UFUNCTION(BlueprintCallable)
		FLinearColor ReturnTextColor() { return text_linear_color_; }

	// 左ページに表示するテキストを返す
	UFUNCTION(BlueprintCallable)
		FString ReturnLeftPageText() { return left_text_; }

	// 右ページに表示するテキストを返す
	UFUNCTION(BlueprintCallable)
		FString ReturnRightPageText() { return right_text_; }
};
