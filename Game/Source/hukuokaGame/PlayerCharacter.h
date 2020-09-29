//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.h
// �T�v			�F�v���C���[�L�����𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.h
// �T�v			�FVR�J�����̍쐬
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/08/18
//-------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// �O���錾
class AItemBase;

// �C�x���g�f�B�X�p�b�`���[�錾
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemCheckBeginEventDispatcher);		// �v���C���[�̎�����CanCheck�A�C�e���ɓ���������
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemCheckEndEventDispatcher);			// �v���C���[�̎�����CanCheck�A�C�e������O�ꂽ��

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
	void UpdateCameraPitch(const float _deltaTime);

	// �J����(Yaw)�̍X�V
	void UpdateCameraYaw(const float _deltaTime);

	// �v���C���[�̈ړ�����
	void UpdatePlayerMove(const float _deltaTime);

	// �A�C�e���`�F�b�N
	void CheckItem();

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
	void PlayerStand() { m_isStanding = true; }
	// �v���C���[�A�N�V�����F���Ⴊ��
	void PlayerSquat() { m_isStanding = false; }

	// �v���C���[�A�N�V�����F�E���A���ׂ�A�쓮������
	void CheckToActor();

public:
	UFUNCTION(BlueprintCallable, Category = "Return State")
		AItemBase* ReturnCheckingItem() const;

	UFUNCTION(BlueprintCallable, Category = "Return State")
		FString ReturnCheckingItemCommandName() const;

private:
	// �C�x���g�f�B�X�p�b�`���[��`
	// �v���C���[�̎�����CanCheck�A�C�e���ɓ���������
	UPROPERTY(BlueprintAssignable)
		FOnItemCheckBeginEventDispatcher  OnItemCheckBeginEventDispatcher;

	// �v���C���[�̎�����CanCheck�A�C�e������O�ꂽ��
	UPROPERTY(BlueprintAssignable)
		FOnItemCheckEndEventDispatcher  OnItemCheckEndEventDispatcher;

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
		class USceneComponent* m_pCameraBase;	// �J�����̌��_

	UPROPERTY(EditAnywhere, Category = "Camera")
		class UCameraComponent* m_pCamera;			// �J����

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_eyeLevelWhenStanding;			// �����Ă���Ƃ��̖ڂ̍���

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputPitch;				// Pitch���씽�]

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputYaw;					// Yaw���씽�]

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraRotateSpeed;				// �J�����̉�]�X�s�[�h

	UPROPERTY(EditAnywhere, Category = "Check")
		float m_CheckToActorRayRange;

	bool isStanding;							// �����Ă��邩�ǂ����̃t���O
	bool isVRCamera;							// VR�J�������ǂ���

	bool m_isStanding;							// �����Ă��邩�ǂ����̃t���O

	float m_playerMoveSpeed;					// �v���C���[�̈ړ����x

	FVector2D m_playerMoveInput;				// �v���C���[�̈ړ����͗�
	FVector2D m_cameraRotateInput;				// �J�����̉�]���͗�

	AItemBase* m_pCheckingItem;					// �`�F�b�N���̃A�C�e��
	AItemBase* m_pPrevCheckItem;				// 1�t���[���O�Ƀ`�F�b�N���Ă����A�C�e��
};