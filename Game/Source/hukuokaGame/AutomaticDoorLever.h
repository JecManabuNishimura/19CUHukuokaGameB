//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.h
// �T�v			�FItemBase�N���X���p�����鎩���h�A�쓮�̃��b�N�A�������Ǘ����郌�o�[
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

class AAutomaticDoorBody;

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorLever : public AItemBase
{
	GENERATED_BODY()
	
public:	
	AAutomaticDoorLever();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮)
	virtual void CheckedByPlayer();

private:
	UPROPERTY(EditAnywhere)
		int lever_filter_num_;				// ���o�[�ƃh�A��Ή������邽�߂̐���

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_when_lever_up_;	// �`�F�b�N���ꂽ���̌��ʉ�

	AAutomaticDoorBody* p_door_body_;	// �Ή�����h�A�{�̂̃|�C���^

	bool is_lever_on_;					// ���o�[��ON/OFF���

	bool can_control_;					// ����\��

public:
	// ���o�[��ON/OFF��Ԃ�Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Get State")
		bool GetLeverState()const { return is_lever_on_; }

	// ���o�[�̑���E�s�t���O��Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Get State")
		bool GetCanControl()const { return can_control_; }

	// ���o�[�̑���E�s�t���O��ݒ肷��
	UFUNCTION(BlueprintCallable, Category = "Set State")
		void SetCanControl(const bool _flag){ can_control_ = _flag; }

	// ���o�[��ON/OFF��Ԃ�ύX(�쐬�ҁ@����)
	UFUNCTION(BlueprintCallable, Category = "Set State")
		void SetLeverState(const bool _flag) { is_lever_on_ = _flag; }

	// ���o�[�̃t�B���^�[�ԍ���Ԃ�
	int GetLeverFilter()const { return lever_filter_num_; }
};