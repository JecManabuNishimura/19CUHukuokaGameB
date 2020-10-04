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

// �O���錾
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
		int m_leverFilter;				// ���o�[�ƃh�A��Ή������邽�߂̐���

	bool m_isLeverOn;					// ���o�[��ON/OFF���

	AAutomaticDoorBody* m_pDoorBody;	// �Ή�����h�A�{�̂̃|�C���^

public:
	// ���o�[��ON/OFF��Ԃ�Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Get State")
		bool GetLeverState()const { return m_isLeverOn; }

	// ���o�[�̃t�B���^�[�ԍ���Ԃ�
	int GetLeverFilter()const { return m_leverFilter; }
};
