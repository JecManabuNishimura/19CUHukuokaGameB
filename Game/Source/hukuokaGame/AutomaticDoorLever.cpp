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

	// プレイヤーにチェックされていたら
	if (m_isChecked)
	{
		// メッシュを白くする
	}
}

// プレイヤーにチェックされたら呼ばれる(作動：状態反転)
void AAutomaticDoorLever::CheckedByPlayer()
{
	// レバーの状態を反転
	m_isLeverOn = !m_isLeverOn;

	// ドア本体の作動フラグを更新
	if (m_pDoorBody != NULL)
	{
		m_pDoorBody->UpdateSwitchState(m_isLeverOn);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
	}
}

