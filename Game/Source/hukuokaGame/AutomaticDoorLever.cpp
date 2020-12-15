//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.cpp
// 概要			：ItemBaseクラスを継承する自動ドア作動のロック、解除を管理するレバー
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------


#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ

AAutomaticDoorLever::AAutomaticDoorLever()
	: m_leverFilter(-1)
	, m_isLeverOn(false)
	, can_control_(true)
	, m_pDoorBody(NULL)
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
			AAutomaticDoorBody* pDoorBody = Cast<AAutomaticDoorBody>(actors[idx]);
			// ドア本体のフィルター番号がレバーのフィルター番号と一致していれば格納する
			if (pDoorBody->GetDoorFilter() == m_leverFilter)
			{
				m_pDoorBody = pDoorBody;
				break;
			}
		}
		// レバーに対応するドアがレベルに配置されていないためエラー
		if (!m_pDoorBody)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	// そもそもドアが一つもレベルに配置されていないためエラー
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no door installed on the level !"));
	}
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
	if (!m_isLeverOn)
	{
		// 動作音を鳴らす
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

		// レバーの状態を反転
		m_isLeverOn = true;

		// ドア本体の作動フラグを更新
		if (m_pDoorBody != NULL)
		{
			m_pDoorBody->UpdateSwitchState(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
}
