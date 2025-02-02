//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.cpp
// 概要			：ItemBaseクラスを継承する自動ドア作動のロック、解除を管理するレバー
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.cpp
// 作成者		：19CU0236 林雲暉 
// 更新日		：2021/03/07			アイテムヒントを追加
//-------------------------------------------------------------------

#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

AAutomaticDoorLever::AAutomaticDoorLever()
	: lever_filter_num_(-1) 
	, p_sound_when_lever_up_(NULL)
	, p_door_body_(NULL)
	, is_lever_on_(false)
	, can_control_(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();

	// フィルター番号が一致するドア取得、保存
	TSubclassOf<AAutomaticDoorBody> findClass;
	findClass = AAutomaticDoorBody::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorBody* p_door_body = Cast<AAutomaticDoorBody>(actors[idx]);
			// ドア本体のフィルター番号がレバーのフィルター番号と一致していれば格納する
			if (p_door_body->GetDoorFilter() == lever_filter_num_)
			{
				p_door_body_ = p_door_body;
				break;
			}
		}
		// レバーに対応するドアがレベルに配置されていないためエラー
		if (!p_door_body_)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	// そもそもドアが一つもレベルに配置されていないためエラー
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no door installed on the level !"));
	}

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
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid (Automatic Door Lever)"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist (Automatic Door Lever)"));
	} // end else

}

void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// プレイヤーにチェックされたら呼ばれる(作動：状態反転)
void AAutomaticDoorLever::CheckedByPlayer()
{
	// レバーの操作が不可ならreturn
	if (!can_control_) return;

	// OFFだったらONに
	if (!is_lever_on_)
	{
		// 動作音を鳴らす(下げる)
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_when_checked_, GetActorLocation());

		// レバーの状態を反転
		is_lever_on_ = true;

		// ミッションに反映する場合、スマホのミッションをアップデート
		if (this->isMissionComplete == false) {

			if (this->items_Mission_Num != 0)
			{
				APlayerCharacter* player_character_;
				player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

				if (player_character_ != NULL)
				{
					player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
				} // end if()
			} // end if()
		} // end if()


		// ドア本体の作動フラグを更新
		if (p_door_body_ != NULL)
		{
			p_door_body_->UpdateSwitchState(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	else
	{
		// 動作音を鳴らす(上げる)
		if (p_sound_when_lever_up_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_when_lever_up_, GetActorLocation());
	}
}
