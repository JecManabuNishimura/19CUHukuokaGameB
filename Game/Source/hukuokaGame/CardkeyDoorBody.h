//-------------------------------------------------------------------
// �t�@�C��		�FCardkeyDoorBody.h
// �T�v			�F�J�[�h�L�[�Ή��̃h�A�𐧌䂷��N���X(�C����:12/11 ItemBase���p�����ăR�}���h�ł�������悤��)
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/22
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"
#include "AutomaticDoorLever.h"
#include "CardkeyDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API ACardkeyDoorBody : public AItemBase
{
	// �h�A�̏��
	enum DOOR_STATE
	{
		kDoorStateLoading = 0,			// �J�[�h�L�[�ǂݍ��ݒ�
		kDoorStateError,				// �ǂݎ�莸�s
		kDoorStateClosed,				// �������Ă���
		kDoorStateOpened,				// �J�������Ă���
		kDoorStateClosing,				// ���r���i�J���j
		kDoorStateOpening,				// �J���r���i���J�j
	};

	GENERATED_BODY()

public:
	// �R���X�g���N�^
	ACardkeyDoorBody();
	// �f�X�g���N�^
	~ACardkeyDoorBody();

protected:
	// ���s���Ɉ�x�Ă΂��
	virtual void BeginPlay() override;

public:
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UFUNCTION()
		void OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
		UBoxComponent* p_door_body_eventtriggerbox_;	// �v���C���[�A�G���ݖh�~�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_cardreader_mesh_1_;	// �J�[�h�ǂݍ��݌��̃��b�V���R���|�[�l���g����1

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_cardreader_mesh_2_;	// �J�[�h�ǂݍ��݌��̃��b�V���R���|�[�l���g����2

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_leftdoor_mesh_;		// ���h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_rightdoor_mesh_;		// �E�h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_door_state_mesh_1_;	// ���h�A�̃��b�N��ԃ��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_door_state_mesh_2_;	// �E�h�A�̃��b�N��ԃ��b�V���R���|�[�l���g

	UMaterial* p_door_state_material_;				// �h�A�̃��b�N��Ԃ�\���}�e���A��

	UMaterialInstanceDynamic* p_material_instance_dynamic_;

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_open_;			// �h�A���J������SE

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_close_;		// �h�A�����鎞��SE

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_standby_;		// ���[�_�[�̏�ԐF�i�ǂݎ��\���j

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_success_;		// ���[�_�[�̏�ԐF�i�ǂݎ�萬�����j

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_loading_;		// ���[�_�[�̏�ԐF�i�ǂݎ�蒆�̎��j

	UPROPERTY(EditAnywhere)
		FLinearColor state_color_error_;		// ���[�_�[�̏�ԐF�i�ǂݎ�莸�s���j

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_loading_success_;	// �ǂݎ�萬�����̉�

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_loading_error_;		// �ǂݎ�莸�s���̉�

	APlayerCharacter* p_player_character_;		// �v���C���[�L�����N�^�[

	UPROPERTY(EditAnywhere)
		float detect_span_;						// �G�A�v���C���[�̌��m�X�p��
	
	UPROPERTY(EditAnywhere)
		float open_and_close_time_;				// �J�ɗv���鎞��

	UPROPERTY(EditAnywhere)
		float check_cardkey_time_;				// �J�[�h�L�[�ǂݎ��Ɋ|���鎞��

	UPROPERTY(EditAnywhere)
		float display_error_time_;				// �J�[�h�L�[�ǂݎ��G���[�̕\���b��

	float count_for_check_display_time_;		// �J�[�h�L�[�ǂݎ��A�G���[�\���̃J�E���g�p

	float left_door_start_posY_;				// ���h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float left_door_end_posY_;				// ���h�A��Y���W�I�_

	float right_door_start_posY_;				// �E�h�A��Y���W�n�_

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 7))
		int door_filter_num_;						// �J�[�h�ƃJ�[�h���[�_�[��Ή������邽�߂̐���

	DOOR_STATE door_state_;						// �h�A�̏��

	AAutomaticDoorLever* p_alert_lever_;		// ��탌�o�[(�~�낳���Ƌ����I�ɊJ��)

	bool is_alert_lever_on_;					// ��탌�o�[���~�낳��Ă��邩

	UPROPERTY(EditAnywhere)
		bool is_doorbody_eventbox_overlap_;		// �h�A�{�̂̃C�x���g�{�b�N�X�Ƀv���C���[�A�G���I�[�o�[���b�v���Ă��邩

	UPROPERTY(EditAnywhere)
		int door_eventbox_overlap_sum_;			// �h�A�{�̂̃C�x���g�{�b�N�X���ɓ����Ă���v���C���[�A�G�̑��a(0��is_door_eventbox_overlap = false)

	float open_time_count_;						// �J���Ă��鎞�Ԃ̃J�E���g

	float required_time_for_open_and_close_;	// �J�̏��v����

	float left_door_dire_for_move_;				// �ړ�����Ƃ��̌���

	float distance_start_to_end_;				// �h�A�̎n�_����I�_�܂ł̋���(�Е��Ŏ擾���A�����Е��ł�����𗘗p)

	void CheckAlertLeverState();				// ��탌�o�[�̏�ԃ`�F�b�N

	void UpdateDoorState(float _deltatime);		// �h�A�̏�ԍX�V

	void UpdateDoorMove(float _deltatime);		// �h�A�̍X�V

	void CheckDetectSpan(float _deltatime);		// �h�A���J����������̌��m�`�F�b�N

public:

	// �h�A�̃t�B���^�[�ԍ���Ԃ�
	int GetDoorFilter()const { return door_filter_num_; }

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(������)
	void CheckedByPlayer();
};
