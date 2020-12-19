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
#include "CardkeyDoorBody.generated.h"

UCLASS()
class HUKUOKAGAME_API ACardkeyDoorBody : public AItemBase
{
	// �h�A�̏��
	enum DOOR_STATE
	{
		DOOR_STATE_LOADING = 0,			// �J�[�h�L�[�ǂݍ��ݒ�
		DOOR_STATE_ERROR,				// �ǂݎ�莸�s
		DOOR_STATE_CLOSED,				// �������Ă���
		DOOR_STATE_OPENED,				// �J�������Ă���
		DOOR_STATE_CLOSING,				// ���r���i�J���j
		DOOR_STATE_OPENING,				// �J���r���i���J�j
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
		UBoxComponent* door_body_eventtriggerbox_;	// �v���C���[�A�G���ݖh�~�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* cardreader_mesh_1_;	// �J�[�h�ǂݍ��݌��̃��b�V���R���|�[�l���g����1

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* cardreader_mesh_2_;	// �J�[�h�ǂݍ��݌��̃��b�V���R���|�[�l���g����2

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* leftdoor_mesh_;		// ���h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* rightdoor_mesh_;		// �E�h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_state_mesh_1_;	// ���h�A�̃��b�N��ԃ��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_state_mesh_2_;	// �E�h�A�̃��b�N��ԃ��b�V���R���|�[�l���g

	UMaterial* door_state_material;					// �h�A�̃��b�N��Ԃ�\���}�e���A��

	UMaterialInstanceDynamic* material_instance_dynamic_;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_open_;			// �h�A���J������SE

	UPROPERTY(EditAnywhere)
		USoundBase* sound_door_close_;			// �h�A�����鎞��SE

	UPROPERTY(EditAnywhere)
		FLinearColor standby_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor success_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor loading_state_color;

	UPROPERTY(EditAnywhere)
		FLinearColor error_state_color;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_loading_success;

	UPROPERTY(EditAnywhere)
		USoundBase* sound_loading_error;

	APlayerCharacter* player_character_;

	UPROPERTY(EditAnywhere)
		float m_detectSpan;						// �G�A�v���C���[�̌��m�X�p��
	
	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// �J�ɗv���鎞��

	UPROPERTY(EditAnywhere)
		float check_cardkey_time_;				// �J�[�h�L�[�ǂݎ��Ɋ|���鎞��

	UPROPERTY(EditAnywhere)
		float display_error_time_;				// �J�[�h�L�[�ǂݎ��G���[�̕\���b��

	float count_for_check_display_time_;		// �J�[�h�L�[�ǂݎ��A�G���[�\���̃J�E���g�p

	float m_leftDoorStartPosY;					// ���h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// ���h�A��Y���W�I�_

	float m_rightDoorStartPosY;					// �E�h�A��Y���W�n�_

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 7))
		int m_doorFilter;						// �J�[�h�ƃJ�[�h���[�_�[��Ή������邽�߂̐���

	DOOR_STATE m_doorState;						// �h�A�̏��

	UPROPERTY(EditAnywhere)
		bool is_doorbody_eventbox_overlap;		// �h�A�{�̂̃C�x���g�{�b�N�X�Ƀv���C���[�A�G���I�[�o�[���b�v���Ă��邩

	UPROPERTY(EditAnywhere)
		int door_eventbox_overlap_sum;			// �h�A�{�̂̃C�x���g�{�b�N�X���ɓ����Ă���v���C���[�A�G�̑��a(0��is_door_eventbox_overlap = false)

	float m_openTimeCount;						// �J���Ă��鎞�Ԃ̃J�E���g

	float m_requiredTime;						// �J�̏��v����

	float m_leftDoorMoveDirection;				// �ړ�����Ƃ��̌���

	float m_distanceStartToEnd;					// �h�A�̎n�_����I�_�܂ł̋���(�Е��Ŏ擾���A�����Е��ł�����𗘗p)

	void UpdateDoorState(float _deltatime);		// �h�A�̏�ԍX�V

	void UpdateDoorMove(float _deltatime);		// �h�A�̍X�V

	void CheckDetectSpan(float _deltatime);		// �h�A���J����������̌��m�`�F�b�N

public:

	// �h�A�̃t�B���^�[�ԍ���Ԃ�
	int GetDoorFilter()const { return m_doorFilter; }

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(������)
	void CheckedByPlayer();
};
