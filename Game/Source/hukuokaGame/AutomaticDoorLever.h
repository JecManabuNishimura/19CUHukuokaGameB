//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.h
// �T�v			�FItemBase�N���X���p�����鎩���h�A�̃��b�N�A�������Ǘ����郌�o�[
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

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

public:
	// ���o�[��ON/OFF��Ԃ�Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnLeverState(){ return m_isLeverOn; }

	UPROPERTY(EditAnywhere)
		int m_leverFilter;					// ���o�[�ƃh�A��Ή������邽�߂̐���

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;					// ���o�[��ON/OFF���
};
