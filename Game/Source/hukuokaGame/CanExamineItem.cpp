//-------------------------------------------------------------------
// ファイル		：CanExamineItem.cpp
// 概要			：ItemBaseクラスを継承するファイル等の調べられるアイテムクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/10/26
//-------------------------------------------------------------------

#include "CanExamineItem.h"

ACanExamineItem::ACanExamineItem()
	: player_character_(NULL)
	, file_mesh_(NULL)
	, datatable_(NULL)
	, transform_on_map_(FTransform::Identity)
	, text_linear_color_(FLinearColor::Transparent)
	, file_kind_(0)
	, page_num_(1)
	, left_page_open_now_num_(-1)
	, time_open_close_(1.f)
	, time_display_text_(2.f)
	, distance_from_file_to_player_(150.f)
	, count_for_time_open_close_(0.f)
	, is_show_details_(false)
	, can_start_anim_(false)
	, can_turn_page_(false)
	, can_show_detial_(true)
	, widget_comp_(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// ファイルのメッシュ生成
	file_mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FileMeshComp"));

	// ウィジェットコンポーネント生成
	widget_comp_ = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));

	if (file_mesh_ != NULL)		file_mesh_->SetupAttachment(RootComponent);
	if (widget_comp_ != NULL)	widget_comp_->SetupAttachment(file_mesh_);
}

void ACanExamineItem::BeginPlay()
{
	// プレイヤーを取得
	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// プレイヤーが取得できなければメッセージ表示
	if (player_character_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// データテーブルがセットされていればデータテーブルの全テキストを取得
	if (datatable_ != NULL)
	{
		TArray<TArray<FString>> datatable_all_text = datatable_->GetTableData();

		// 指定したファイルの最大列まで行くか、何も入力されていないセルに当たるまでテキストを取得、格納
		for (int i = 1, datatable_row_length = datatable_all_text[file_kind_].Num(); i < datatable_row_length && datatable_all_text[file_kind_][i] != ""; ++i)
		{
			text_in_file_kind_.Add(datatable_all_text[file_kind_][i]);
		}

		// 有効なページがあれば総ページ数を設定
		if (text_in_file_kind_[0] != "@")
		{
			page_num_ = text_in_file_kind_.Num();
		}
		// 無ければ削除
		else	this->Destroy();
	
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("DataTable Not be Found !"));

	// マップに置かれている時のトランスフォームを格納
	transform_on_map_ = GetActorTransform();

	// 初期テキスト表示
	TurnPage();

	Super::BeginPlay();
}

void ACanExamineItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	count_for_time_open_close_ += DeltaTime;

	// 調べられているならカウント
	if (is_show_details_)
	{
		if (count_for_time_open_close_ > time_open_close_)
		{
			// 開くアニメーションの許可フラグを立てる
			can_start_anim_ = true;
			
			// テキストのフェードイン処理
			text_linear_color_ = FLinearColor(0, 0, 0, (count_for_time_open_close_ - time_display_text_));

			// テキストが完全に不透明になったらめくる許可フラグを立てる
			if (count_for_time_open_close_ > (time_display_text_ + 1.0f))
			{
				text_linear_color_ = FLinearColor(0.f, 0.f, 0.f, 1.f);
				can_turn_page_ = true;
			}
		}
	}
	else
	{
		if (!can_show_detial_)
		{
			if (count_for_time_open_close_ > time_open_close_)
			{
				if (abs(GetActorLocation().X - transform_on_map_.GetLocation().X) > 10.f)
				{
					// プレイヤーの操作を有効に
					player_character_->SetPlayerControlFlag(true);

					SetActorTransform(transform_on_map_);

					can_show_detial_ = true;
				}
			}
		}
	}
}

// プレイヤーからのチェックをされた(調べられた)
void ACanExamineItem::CheckedByPlayer()
{
	if (can_show_detial_)
	{
		// まだ調べられていないならプレイヤーの目の前に表示
		if (is_show_details_ == false)
		{
			// プレイヤーの操作を不可にする
			player_character_->SetPlayerControlFlag(false);

			// 調べているフラグを立てる
			is_show_details_ = true;

			count_for_time_open_close_ = 0.0f;

			// プレイヤーの目の前に表示
			SetActorLocation(player_character_->ReturnCameraLocation() + (player_character_->ReturnCameraForwardVector() * distance_from_file_to_player_));
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player_character_->ReturnCameraLocation()));
		}
		else
		{
			// ページをめくる許可が出ていればテキスト更新
			if (can_turn_page_)	TurnPage();

			// テキスト切り替え
			OnFileTrunPageEventDispatcher.Broadcast();
		}
	}
}

// ページをめくる
void ACanExamineItem::TurnPage()
{
	// 次の左ページが有効ならテキスト切り替え
	if ((left_page_open_now_num_ + 1) < page_num_)
	{
		++left_page_open_now_num_;
		left_text_ = text_in_file_kind_[left_page_open_now_num_];

		++left_page_open_now_num_;

		// 次の右ページが有効ならテキスト切り替え
		if ((left_page_open_now_num_) < page_num_)
		{
			right_text_ = text_in_file_kind_[left_page_open_now_num_];
		}
		// 無効なら空白1文字に
		else
		{
			right_text_ = " ";
		}
	}
	else
	{
		//------------------------------------------------
		// 本を閉じてマップに戻す
		//------------------------------------------------

		// テキストを透明に
		text_linear_color_ = FLinearColor(0, 0, 0, 0);

		// 開いているページ情報を初期化し更新
		left_page_open_now_num_ = -1;
		TurnPage();

		// ファイルを開いているフラグを降ろす
		can_start_anim_ = false;

		is_show_details_ = false;

		can_turn_page_ = false;

		can_show_detial_ = false;

		count_for_time_open_close_ = 0.0f;
	}
}