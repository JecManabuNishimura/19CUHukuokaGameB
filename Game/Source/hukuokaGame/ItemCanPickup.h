//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.h
// �T�v			�FItemBase�N���X���p������E�����Ƃ��o����A�C�e��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemCanPickup.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemCanPickup : public AItemBase
{
	GENERATED_BODY()

public:
	AItemCanPickup();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮)
	virtual void CheckedByPlayer();

public:
};
