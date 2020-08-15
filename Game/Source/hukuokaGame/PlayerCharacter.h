//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.h
// �T�v			�F�v���C���[�L�����𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/07
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class HUKUOKAGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// �R���X�g���N�^
	APlayerCharacter();
	// �f�X�g���N�^
	~APlayerCharacter();

protected:
	// ���s���Ɉ�x�Ă΂��
	virtual void BeginPlay() override;

public:	
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

	// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	// �n�ʂ̂Ƃ̋����𑪂�v���C���[�̍�����ݒ�
	void SetEyeLevel(const float _deltaTime);

	// �J����(Pitch)�̍X�V
	void UpdateCameraPitch();

	// �J����(Yaw)�̍X�V
	void UpdateCameraYaw();

	// �v���C���[�̈ړ�����
	void UpdatePlayerMove(const float _deltaTime);

	// �x�N�g���̒�����Ԃ�
	float ReturnVector2DLength(const FVector2D* _pFvector2d);

	// �x�N�g���𐳋K������
	void NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d);

private:
	// ���̓o�C���h
	// �J������]�FPitch(Y��)
	void CameraRotatePitch(float _axisValue);
	// �J������]�FYaw(Z��)
	void CameraRotateYaw(float _axisValue);

	// �v���C���[�ړ��F�ړ�X������(�c)
	void PlayerMoveX(float _axisValue);
	// �v���C���[�ړ��F�ړ�Y������(��)
	void PlayerMoveY(float _axisValue);

	// �v���C���[�A�N�V�����F����������
	void PlayerStand() { isStanding = true; };
	// �v���C���[�A�N�V�����F���Ⴊ��
	void PlayerSquat() { isStanding = false; };

private:
	// �v���p�e�B
	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerThresholdToRun;			// ����臒l(0 < this <= 1)

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerRunSpeed;					// �v���C���[�̕������x

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerWalkSpeed;				// �v���C���[�̑��鑬�x

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMin;			// �J������Pitch�����ŏ��p�x

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMax;			// �J������Pitch�����ő�p�x

	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* m_pCamera;			// �J����

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_eyeLevelWhenStanding;			// �����Ă���Ƃ��̖ڂ̍���

	bool isStanding;							// �����Ă��邩�ǂ����̃t���O

	float m_playerMoveSpeed;					// �v���C���[�̈ړ����x

	FVector2D m_playerMoveInput;				// �v���C���[�̈ړ����͗�
	FVector2D m_cameraRotateInput;				// �J�����̉�]���͗�
};