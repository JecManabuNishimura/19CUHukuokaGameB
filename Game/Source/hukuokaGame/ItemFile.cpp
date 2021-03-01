//-------------------------------------------------------------------
// ファイル		：ItemFile.cpp
// 概要			：ItemFileクラスを継承するページをめくることが出来るファイル
// 作成者		：19CU0233 増井悠斗
// 作成日		：2021/02/10
//-------------------------------------------------------------------

#include "ItemFile.h"

AItemFile::AItemFile()
	: p_file_mesh_(NULL)
	, p_datatable_(NULL)
	, p_sound_when_turnpage_(NULL)
	, file_kind_(0)
	, time_open_close_(1.f)
	, time_display_text_(2.f)
	, distance_from_file_to_player_(150.f)
	, p_player_character_(NULL)
	, transform_on_map_(FTransform::Identity)
	, text_linear_color_(FLinearColor::Transparent)
	, left_text_("")
	, right_text_("")
	, page_num_(0)
	, left_page_open_now_num_(-1)
	, count_for_time_open_close_(0.f)
	, is_show_details_(false)
	, can_start_anim_(false)
	, can_turn_page_(false)
	, can_show_detial_(true)
	, do_file_loc_correction_(false)
	, p_widget_comp_(NULL)

{
	// 動的配列の初期化
	text_in_file_kind_.Reset();

	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// ファイルのメッシュ生成
	p_file_mesh_ = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FileMeshComp"));

	// ウィジェットコンポーネント生成
	p_widget_comp_ = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));

	if (p_file_mesh_ != NULL)	p_file_mesh_->SetupAttachment(RootComponent);
	if (p_widget_comp_ != NULL)	p_widget_comp_->SetupAttachment(p_file_mesh_);
}

void AItemFile::BeginPlay()
{
	// プレイヤーを取得
	p_player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// プレイヤーが取得できなければメッセージ表示
	if (p_player_character_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// データテーブルがセットされていればファイルに対応したテキストを移す
	if (p_datatable_ != NULL)
	{
		TArray<FFileTextStruct*> datatable_struct_;

		// rowのデータ(struct)*行の(テーブル全部の)データを取得
		p_datatable_->GetAllRows<FFileTextStruct>(FString(), datatable_struct_);

		// 要素数0のデータには挿入できないので1を設定
		text_in_file_kind_.SetNum(1);

		// 構造体に設定されているファイル番号を指定していたらテキストを設定
		if (file_kind_ <= p_datatable_->GetRowNames().Num())
		{
			// 空データでも構わずに挿入し要素数 11 (= 10 + 1)個のデータを作る
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page10, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page9, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page8, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page7, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page6, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page5, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page4, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page3, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page2, 0);
			text_in_file_kind_.Insert(datatable_struct_[file_kind_ - 1]->page1, 0);

			// 空データの有無でページ数を設定
			for (page_num_ = 0; text_in_file_kind_[page_num_] != ""; ++page_num_);

			// 最初に不正なアクセスを回避するために設定した 11 番目のデータを削除
			text_in_file_kind_.SetNum(page_num_);

			// マップに置かれている時のトランスフォームを格納
			transform_on_map_ = GetActorTransform();

			// 初期テキストに更新
			UpdatePage(false);
		}
		else this->Destroy();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("DataTable Not be Found !"));
		this->Destroy();
	}

	Super::BeginPlay();
}

void AItemFile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (do_file_loc_correction_)
	{
		do_file_loc_correction_ = false;

		// プレイヤーの目の前に表示
		SetActorLocation(p_player_character_->GetCameraLocation() + (p_player_character_->GetCameraForwardVector() * distance_from_file_to_player_));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), p_player_character_->GetCameraLocation()));
	}

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
					p_player_character_->SetPlayerMoveControlFlag(true);
					p_player_character_->SetPlayerCameraControlFlag(true);
					p_player_character_->TakeOutTheSmartPhone(true);			// スマホを使かえる追加(作成者:林雲暉)

					SetActorTransform(transform_on_map_);

					can_show_detial_ = true;
				}
			}
		}
	}
}

// プレイヤーからのチェックをされた(調べられた)
void AItemFile::CheckedByPlayer()
{
	if (can_show_detial_)
	{
		// まだ調べられていないならプレイヤーの目の前に表示
		if (is_show_details_ == false)
		{
			// 取得音を鳴らす
			if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

			// プレイヤーの操作を不可にする
			p_player_character_->SetPlayerMoveControlFlag(false);
			p_player_character_->SetPlayerCameraControlFlag(false);
			p_player_character_->TakeOutTheSmartPhone(false);			// スマホを使えない追加(作成者:林雲暉)

			// 調べているフラグを立てる
			is_show_details_ = true;

			// 位置補正フラグを立てる
			do_file_loc_correction_ = true;

			count_for_time_open_close_ = 0.0f;

			// テキスト更新
			OnFileUpdatePageEventDispatcher.Broadcast();
		}
		else
		{
			// ページをめくる許可が出ていればテキスト更新
			if (can_turn_page_)	UpdatePage(true);

			// テキスト更新
			OnFileUpdatePageEventDispatcher.Broadcast();
		}
	}
}

// ページをめくる
void AItemFile::UpdatePage(const bool _make_sound)
{
	// 次の左ページが有効ならテキスト切り替え
	if ((left_page_open_now_num_ + 1) < page_num_)
	{
		// ページをめくる音を鳴らす
		if (p_sound_when_turnpage_ != NULL && _make_sound)	UGameplayStatics::PlaySound2D(GetWorld(), p_sound_when_turnpage_);

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

		// TurnPage();
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

		// ファイルを開いているフラグを降ろす
		can_start_anim_ = false;

		is_show_details_ = false;

		can_turn_page_ = false;

		can_show_detial_ = false;

		count_for_time_open_close_ = 0.0f;
	}
}
