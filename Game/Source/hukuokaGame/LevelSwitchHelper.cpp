//-------------------------------------------------------------------
// �t�@�C��		�FLevelSwitchHelper.cpp
// �T�v			�F���x���J�ڗp�N���X
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/10/09
//-------------------------------------------------------------------


#include "LevelSwitchHelper.h"
#include "Kismet/GameplayStatics.h"

// �ÓI�֐��A�^�C�g���}�b�v�ɑJ��
void LevelSwitchHelper::OpenTitleLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "Title");
}

// �ÓI�֐��A�Q�[���}�b�v�ɑJ��
void LevelSwitchHelper::OpenProtoTypeLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "ProtoType");
}

// �ÓI�֐��A�Q�[���I�[�o�[�ɑJ��
void LevelSwitchHelper::OpenGameOverLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "GameOver");
}

// �ÓI�֐��A�Q�[���^�C�g���ɑJ��
void LevelSwitchHelper::OpenGameClearLevelMap(const UObject* worldContent)
{
	UGameplayStatics::OpenLevel(worldContent, "GameClear");
}

