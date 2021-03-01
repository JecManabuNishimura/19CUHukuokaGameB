//-------------------------------------------------------------------
// ファイル		：ItemFile.h
// 概要			：ItemFileクラスを継承するページをめくることが出来るファイル
// 作成者		：19CU0233 増井悠斗
// 作成日		：2021/02/10
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
#include "ItemFile.generated.h"

// イベントディスパッチャー宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFileUpdatePageEventDispatcher);		// ページをめくる(=テキストUIの更新)

// ファイルの構造体(string型10個)
USTRUCT(BlueprintType)
struct FFileTextStruct : public FTableRowBase
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
class HUKUOKAGAME_API AItemFile : public AItemBase
{
	GENERATED_BODY()

public:
	AItemFile();

	virtual ~AItemFile() {}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintAssignable)
		FOnFileUpdatePageEventDispatcher  OnFileUpdatePageEventDispatcher;

	// ファイルのスケルタルメッシュ
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* p_file_mesh_;

	UPROPERTY(EditAnywhere)
		UDataTable* p_datatable_;			// ファイルのテキストデータ

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_when_turnpage_;	// チェックされた時の効果音

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int file_kind_;					// ファイルの識別番号

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_open_close_;			// 調べてからアニメーションし始めるまでの秒数

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_display_text_;		// 調べてからテキストがフェードインし始めるまでの秒数

	UPROPERTY(EditAnywhere)
		float distance_from_file_to_player_;	// 調べたときのプレイヤーからファイルまでの距離

private:
	APlayerCharacter* p_player_character_;		// プレイヤー

	FTransform transform_on_map_;		// マップ上に配置されている時の情報(調べ終えたときに利用)

	FLinearColor text_linear_color_;	// テキストのカラー

	TArray<FString> text_in_file_kind_;	// 指定したファイル番号の全テキストデータ格納用変数 

	FString left_text_;					// 左ページに表示する文字

	FString right_text_;				// 右ページに表示する文字

	int page_num_;						// 総ページ数

	int left_page_open_now_num_;		// 表示中の左ページ数

	float count_for_time_open_close_;	// アニメーション、テキストフェードイン管理用のカウント

	bool is_show_details_;				// プレイヤーにチェックされているかのフラグ

	bool can_start_anim_;				// アニメーション開始許可のフラグ

	bool can_turn_page_;				// ページをめくる許可フラグ

	bool can_show_detial_;				// 戻そうとするときに再度調べてしまうのを防止するためのフラグ

	bool do_file_loc_correction_;		// ファイル位置補正用フラグ

	void UpdatePage(const bool _make_sound);

public:
	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		UWidgetComponent* p_widget_comp_;	// エディタ上で生成したものを取得

public:
	virtual void Tick(float DeltaTime) override;

	// プレイヤーにチェックされたら呼ばれる(調べる)
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
