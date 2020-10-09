//-------------------------------------------------------------------
// �t�@�C��		�FLevelSwitchHelper.h
// �T�v			�F���x���J�ڗp�N���X
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/10/09
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"


class HUKUOKAGAME_API LevelSwitchHelper
{
private:
public:
	LevelSwitchHelper();
	~LevelSwitchHelper();


	// �ÓI�֐��A�^�C�g���}�b�v�ɑJ��
	// �����F
	// UObject* worldContent		���[���h�R���e���g
	static void OpenTitleLevelMap(const UObject* worldContent);

	// �ÓI�֐��A�Q�[���}�b�v�ɑJ��
	// �����F
	// UObject* worldContent		���[���h�R���e���g
	static void OpenProtoTypeLevelMap(const UObject* worldContent);

	// �ÓI�֐��A�Q�[���I�[�o�[�ɑJ��
	// �����F
	// UObject* worldContent		���[���h�R���e���g
	static void OpenGameOverLevelMap(const UObject* worldContent);

	// �ÓI�֐��A�Q�[���N���A�ɑJ��
	// �����F
	// UObject* worldContent		���[���h�R���e���g
	static void OpenGameClearLevelMap(const UObject* worldContent);
};
