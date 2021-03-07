//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.h
// �T�v			�F�E����A���ׂ���A�쓮�ł���Actor�̊�{�N���X(Map��̃A�C�e��)
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.h
// �쐬��		�F19CU0217 ��K
// �X�V��		�F2020/10/14		���g�̕\���̒ǉ�
// �X�V��		�F2020/12/12		SetOutline�̃I�u�W�F�N�g���w��ł���悤�ɂ���
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.h
// �쐬��		�F19CU0236 �щ_�� 
// �X�V��		�F2020/11/28		�A�C�e���Ƀ~�b�V������Ή�
//				�F2021/03/06		�A�C�e���q���g��ǉ�
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "ItemBase.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemBase();

	virtual ~AItemBase() {}

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// �v���C���[�̃`�F�b�N���󂯂�
	virtual void CheckedByPlayer() {}

protected:
	UPROPERTY(EditAnywhere)
		USoundBase* sound_when_checked_;	// �`�F�b�N���ꂽ���̌��ʉ�

	UPROPERTY(EditAnywhere)
		FString command_name_;				// �R�}���h�̖��O(���)

	UPROPERTY(EditAnywhere)
		bool can_checked_when_player_chased_;	// �v���C���[���G�ɒǂ��Ă���Ƃ��Ƀ`�F�b�N�ł���A�C�e����

protected:
	bool is_checked_;						// �v���C���[�Ƀ`�F�b�N����Ă��邩

public:
	// �v���C���[�ɂ���`�F�b�N��Ԃ�ݒ�
	void SetCheckedState(const bool _new_state) { is_checked_ = _new_state; }

	// �v���C���[���G�ɒǂ��Ă���Ƃ��Ƀ`�F�b�N�o����A�C�e�����ǂ�����Ԃ�
	bool GetCanCheckedWhenPlayerChased()const { return can_checked_when_player_chased_; }

	// �R�}���h����Ԃ�
	FString GetCommandName()const { return command_name_; }

public:
	// �v���C���[�ɂ���`�F�b�N��Ԃ�Ԃ�
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnCheckedState()const { return is_checked_; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int items_Mission_Num;					// �����Ă���~�b�V�����i���o�[�A�Ȃ��Ȃ�0	(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool isMissionComplete;					// �Ή������~�b�V�����������܂�����			(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		float infoWorkingHeight;				// �v���C���[�ڋ߂̎��A�C�e���q���g�̍���	(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		float toPlayers_MinDistance;			// �v���C���[�ڋߔ���̋���					(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		float infoScale;						// �A�C�e���q���g�̑傫��					(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		bool isNeedToDiaplsy;					// �A�C�e���q���g��\�����邩				(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		FVector infoPosition;					// �A�C�e���q���g�̈ʒu�����p				(�쐬��:�щ_��)

	// �A�C�e���q���g��Actor	(�쐬��:�щ_��)
	UPROPERTY(VisibleAnywhere, Category = "ItemInfo")
		AActor* itemInfoActor;

protected:
	// �A�C�e���q���g�̃I�u�W�F�N�g	(�쐬��:�щ_��)
	TSubclassOf<class AActor> bp_ItemInfo;

public:
	void SetOutline(bool _isCustomDepthOn, const int _checking_comp_index);
};
