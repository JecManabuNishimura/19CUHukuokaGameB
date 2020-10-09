//-------------------------------------------------------------------
// ファイル		：LevelSwitchHelper.h
// 概要			：レベル遷移用クラス
// 作成者		：19CU0217 朱適
// 作成日		：2020/10/09
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"


class HUKUOKAGAME_API LevelSwitchHelper
{
private:
public:
	LevelSwitchHelper();
	~LevelSwitchHelper();


	// 静的関数、タイトルマップに遷移
	// 引数：
	// UObject* worldContent		ワールドコンテント
	static void OpenTitleLevelMap(const UObject* worldContent);

	// 静的関数、ゲームマップに遷移
	// 引数：
	// UObject* worldContent		ワールドコンテント
	static void OpenProtoTypeLevelMap(const UObject* worldContent);

	// 静的関数、ゲームオーバーに遷移
	// 引数：
	// UObject* worldContent		ワールドコンテント
	static void OpenGameOverLevelMap(const UObject* worldContent);

	// 静的関数、ゲームクリアに遷移
	// 引数：
	// UObject* worldContent		ワールドコンテント
	static void OpenGameClearLevelMap(const UObject* worldContent);
};
