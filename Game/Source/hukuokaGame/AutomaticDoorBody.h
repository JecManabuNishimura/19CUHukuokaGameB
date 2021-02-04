//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorBody.h
// �T�v			�F�����h�A�{�̂𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/04
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorBody : public AActor
{
	// �h�A�̏��
	enum DOOR_STATE
	{
		kDoorStateClosed = 0,			// �������Ă���
		kDoorStateOpened,				// �J�������Ă���
		kDoorStateClosing,				// ���r���i�J���j
		kDoorStateOpening,				// �J���r���i���J�j
	};

	GENERATED_BODY()

public:
	// �R���X�g���N�^
	AAutomaticDoorBody();
	// �f�X�g���N�^
	~AAutomaticDoorBody();

protected:
	// ���s���Ɉ�x�Ă΂��
	virtual void BeginPlay() override;

public:
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);

	UPROPERTY(EditAnywhere)
		UBoxComponent* p_eventTriggerBox_;		// �v���C���[�A�G���m�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_left_door_comp_;	// ���h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_right_door_comp_;	// �E�h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* p_lamp_for_decide_pos_;	// �������郉���v�̈ʒu�����߂邽�߂̃����v���b�V��

	UPROPERTY(EditAnywhere)
		UStaticMesh* p_lever_state_lamp_;			// ���o�[�̏�Ԃ�\�������v���b�V��

	UPROPERTY(EditAnywhere)
		UMaterial* p_lever_on_state_material_;	// ���o�[���I���̎��̃}�e���A��

	UPROPERTY(EditAnywhere)
		UMaterial* p_lever_off_state_material_;	// ���o�[���I�t�̎��̃}�e���A��

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_open_;			// �h�A���J������SE

	UPROPERTY(EditAnywhere)
		USoundBase* p_sound_door_close_;		// �h�A�����鎞��SE

	UPROPERTY(EditAnywhere)
		float detect_span_;						// �G�A�v���C���[�̌��m�X�p��

	UPROPERTY(EditAnywhere)
		float open_and_close_time_;				// �J�ɗv���鎞��

	float left_door_start_posY_;				// ���h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float left_door_end_posY_;				// ���h�A��Y���W�I�_

	float right_door_start_posY_;				// �E�h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		int door_filter_num_;					// ���o�[�ƃh�A��Ή������邽�߂̐���

	FVector lamp_generate_pos_;					// �����v�𐶐�����ꏊ

	bool is_switch_on_;							// �����h�A���쓮�����邽�߂̃t���O

	bool is_overlap_;							// �v���C���[�������͓G�����m���ɓ����Ă��邩�̃t���O

	float open_time_count_;						// �J���Ă��鎞�Ԃ̃J�E���g

	float required_time_for_open_and_close_;	// �J�̏��v����

	float left_door_dire_for_move_;				// �ړ�����Ƃ��̌���

	float distance_start_to_end_;				// �h�A�̎n�_����I�_�܂ł̋���(�Е��Ŏ擾���A�����Е��ł�����𗘗p)

	int detect_num_;							// ���m�͈͓�����v���C���[�A�G�̑��a

	DOOR_STATE door_state_;						// �h�A�̏��

	TArray<AAutomaticDoorLever*> filter_match_levers_;	// �Ή����郌�o�[��ۑ�����z��

	int filter_match_levers_num_;				// �Ή����郌�o�[�̐�

	TArray<UStaticMeshComponent*> match_lever_state_lamps_;	// �Ή����郌�o�[�̏�Ԃ�\�������v���b�V��

	void UpdateDoorState();						// �h�A�̏�ԍX�V

	void UpdateDoorMove(float _deltaTime);		// �h�A�̍X�V

	void CheckDetectSpan(float _deltaTime);		// �h�A���J����������̌��m�`�F�b�N

	void GoToPlayerCharacterAndUpdateMission();	// �v���C���[��UpdateTheMission���Ăяo�����߂̃��]�b�g (�쐬��:�щ_��)

public:
	// �h�A�{�̂̃X�C�b�`�̍X�V
	void UpdateSwitchState(const AAutomaticDoorLever* const operated_lever = nullptr);

	// �h�A�̃t�B���^�[�ԍ���Ԃ�
	int GetDoorFilter()const { return door_filter_num_; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int items_Mission_Num;					// �����Ă���~�b�V�����i���o�[�A�Ȃ��Ȃ�0 (�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int next_Items_Mission_Num;				// ���̃~�b�V�����i���o�[�A�Ȃ��Ȃ�0		(�쐬��:�щ_��)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool isMissionComplete;					// �Ή������~�b�V�����������܂�����			(�쐬��:�щ_��)

};
