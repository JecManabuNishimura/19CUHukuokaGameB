//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorBody.h
// �T�v			�F�����h�A�{�̂𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/04
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutomaticDoorBody.generated.h"

// �O���錾
class UBoxComponent;
class UStaticMeshComponent;
class AAutomaticDoorLever;

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorBody : public AActor
{
	// �h�A�̏��
	enum DOOR_STATE
	{
		DOOR_STATE_CLOSED = 0,			// �������Ă���
		DOOR_STATE_OPENED,				// �J�������Ă���
		DOOR_STATE_CLOSING,				// ���r���i�J���j
		DOOR_STATE_OPENING,				// �J���r���i���J�j
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
		UBoxComponent* m_pEventTriggerBox;		// �v���C���[�A�G���m�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pLeftDoorComp;	// ���h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRightDoorComp;	// �E�h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		float m_detectSpan;						// �G�A�v���C���[�̌��m�X�p��

	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// �J�ɗv���鎞��

	float m_leftDoorStartPosY;					// ���h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// ���h�A��Y���W�I�_

	float m_rightDoorStartPosY;					// �E�h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		int m_doorFilter;						// ���o�[�ƃh�A��Ή������邽�߂̐���

	bool m_isSwitchOn;							// �����h�A���쓮�����邽�߂̃t���O

	bool m_isOverlap;							// �v���C���[�������͓G�����m���ɓ����Ă��邩�̃t���O

	float m_openTimeCount;						// �J���Ă��鎞�Ԃ̃J�E���g

	float m_requiredTime;						// �J�̏��v����

	float m_leftDoorMoveDirection;				// �ړ�����Ƃ��̌���

	float m_distanceStartToEnd;					// �h�A�̎n�_����I�_�܂ł̋���(�Е��Ŏ擾���A�����Е��ł�����𗘗p)

	int m_detectNum;							// ���m�͈͓�����v���C���[�A�G�̑��a

	DOOR_STATE m_doorState;						// �h�A�̏��

	TArray<AAutomaticDoorLever*> m_filterMatchLevers;	// (�Ή����郌�o�[)m_doorFilter��ۑ�����z��

	void UpdateDoorState();						// �h�A�̏�ԍX�V

	void UpdateDoorMove(float _deltaTime);		// �h�A�̍X�V

	void CheckDetectSpan(float _deltaTime);		// �h�A���J����������̌��m�`�F�b�N

public:
	// �h�A�{�̂̃X�C�b�`�̍X�V
	void UpdateSwitchState(const bool _isLeverOn = true);

	// �h�A�̃t�B���^�[�ԍ���Ԃ�
	int GetDoorFilter()const { return m_doorFilter; }
};
