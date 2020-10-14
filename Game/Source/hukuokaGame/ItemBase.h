//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.h
// �T�v			�F�E����A���ׂ���A�쓮�ł���Actor�̊�{�N���X(Map��̃A�C�e��)
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.h
// �쐬��		�F19CU0217 ��K
// �X�V��		�F2020/10/14		���g�̕\���̒ǉ�
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �v���C���[�̃`�F�b�N���󂯂�
	virtual void CheckedByPlayer() {}

public:
	// �v���C���[�ɂ���`�F�b�N��Ԃ�Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnCheckedState() { return m_isChecked; }
public:
	bool m_isChecked;			// �v���C���[�Ƀ`�F�b�N����Ă��邩

	UPROPERTY(EditAnywhere)
	FString m_commandName;		// �R�}���h�̖��O(���)

	void SetOutline(bool _isCustomDepthOn);
};
