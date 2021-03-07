//-------------------------------------------------------------------
// ファイル		：ItemCanExamine.cpp
// 概要			：ItemBaseクラスを継承するファイル等の調べられるアイテムクラス
// 作成者		：19CU0233 増井悠斗
// 作成日		：2021/02/10
//-------------------------------------------------------------------

#include "ItemCanExamine.h"

AItemCanExamine::AItemCanExamine()
	: p_photo_mesh_(NULL)
	, distance_from_file_to_player_(150.f)
	, p_playercharacter_(NULL)
	, transform_on_map_(FTransform::Identity)
	, is_show_details_(false)
	, do_file_loc_correction_(false)
{
	p_photo_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhotoMeshComp"));

	if (p_photo_mesh_ != NULL) p_photo_mesh_->SetupAttachment(RootComponent);
}

void AItemCanExamine::BeginPlay()
{
	// プレイヤーを取得
	p_playercharacter_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// プレイヤーが取得できなければメッセージ表示
	if (p_playercharacter_ == NULL) GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, TEXT("PlayerCharacter Not be Found !"));

	// マップに置かれている時のトランスフォームを格納
	transform_on_map_ = GetActorTransform();

	Super::BeginPlay();

	// アイテムヒントを生成する (作成者:林雲暉)
	bp_ItemInfo = TSoftClassPtr<AActor>(FSoftObjectPath("Blueprint'/Game/Blueprints/BP_ItemHint3D.BP_ItemHint3D_C'")).LoadSynchronous();	// pathにあるクラスを取得
	if (bp_ItemInfo != nullptr && isNeedToDiaplsy == true)
	{
		itemInfoActor = GetWorld()->SpawnActor<AActor>(bp_ItemInfo);						// アイテムヒントをActorとして生成する

		if (itemInfoActor != NULL)
		{

			itemInfoActor->SetActorEnableCollision(false);

			itemInfoActor->SetActorLocation((this->GetActorLocation() + infoPosition));

			FOutputDeviceNull ar;
			FString FuncName_and_Solution1 = FString::Printf(TEXT("InitialHeight "));
			FString FuncName_and_Solution2 = FString::Printf(TEXT("InitialScale "));
			FString FuncName_and_Solution3 = FString::Printf(TEXT("InitialDistance "));

			FuncName_and_Solution1 += FString::SanitizeFloat(infoWorkingHeight);
			FuncName_and_Solution2 += FString::SanitizeFloat(infoScale);
			FuncName_and_Solution3 += FString::SanitizeFloat(toPlayers_MinDistance);

			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution1, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution2, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution3, ar, NULL, true);

		} // end if()
		else {
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid (ItemCanExamine)"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist (ItemCanExamine)"));
	} // end else
}

void AItemCanExamine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (do_file_loc_correction_)
	{
		do_file_loc_correction_ = false;

		// プレイヤーの目の前に表示
		SetActorLocation(p_playercharacter_->GetCameraLocation() + (p_playercharacter_->GetCameraForwardVector() * distance_from_file_to_player_));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), p_playercharacter_->GetCameraLocation()));
	}
}

void AItemCanExamine::CheckedByPlayer()
{
	// 現在調べられていないならプレイヤーの目の前に表示
	if (!is_show_details_)
	{
		// 取得音を鳴らす
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

		// プレイヤーの操作を不可にする
		p_playercharacter_->SetPlayerMoveControlFlag(false);
		p_playercharacter_->SetPlayerCameraControlFlag(false);
		p_playercharacter_->TakeOutTheSmartPhone(false);

		// 調べているフラグを立てる
		is_show_details_ = true;

		// 位置補正フラグを立てる
		do_file_loc_correction_ = true;
	}
	// 現在調べられているなら元の位置に戻す
	else
	{
		// プレイヤーの操作を有効に
		p_playercharacter_->SetPlayerMoveControlFlag(true);
		p_playercharacter_->SetPlayerCameraControlFlag(true);
		p_playercharacter_->TakeOutTheSmartPhone(true);

		// 調べているフラグを降ろす
		is_show_details_ = false;

		SetActorTransform(transform_on_map_);
	}
}
