//-------------------------------------------------------------------
// ファイル		：LevelSwitchHelper.cpp
// 概要			：レベル遷移用クラス
// 作成者		：19CU0217 朱適
// 作成日		：2020/10/09
//-------------------------------------------------------------------


#include "LevelSwitchHelper.h"
#include "Kismet/GameplayStatics.h"

// 静的関数、タイトルマップに遷移
void LevelSwitchHelper::OpenTitleLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "Title");
}

// 静的関数、ゲームマップに遷移
void LevelSwitchHelper::OpenProtoTypeLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "ProtoType");
}

// 静的関数、ゲームオーバーに遷移
void LevelSwitchHelper::OpenGameOverLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "GameOver");
}

// 静的関数、ゲームタイトルに遷移
void LevelSwitchHelper::OpenGameClearLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "GameClear");
}

