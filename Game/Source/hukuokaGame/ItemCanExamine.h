//-------------------------------------------------------------------
// �t�@�C��		�FItemCanExamine.h
// �T�v			�FItemBase�N���X���p������t�@�C�����̒��ׂ���A�C�e���N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2021/02/10
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ItemCanExamine.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemCanExamine : public AItemBase
{
	GENERATED_BODY()

public:
	AItemCanExamine();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_photo_mesh_;	// �ʐ^�̃��b�V��

	UPROPERTY(EditAnywhere)
		float distance_from_file_to_player_;	// ���ׂ��Ƃ��̃v���C���[����t�@�C���܂ł̋���
private:
	// �v���C���[
	APlayerCharacter* p_playercharacter_;

	FTransform transform_on_map_;		// �}�b�v��ɔz�u����Ă��鎞�̏��(���׏I�����Ƃ��ɗ��p)

	bool is_show_details_;				// ���ݒ��ׂ��Ă��邩

	bool do_file_loc_correction_;		// �t�@�C���ʒu�␳�p�t���O

public:
	virtual void Tick(float DeltaTime) override;

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(���ׂ�)
	void CheckedByPlayer();

};