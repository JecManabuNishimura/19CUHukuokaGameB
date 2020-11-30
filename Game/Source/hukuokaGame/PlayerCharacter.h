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
// �X�V��		�F2020/11/04		�S�����A�v���̃A�N�V�����}�b�s���O��ǉ�
// �X�V��		�F2020/11/05		�C���X�^���X�����邩�ǂ����̊m�F��ǉ�
// �X�V��		�F2020/11/16		�_���[�W��ԂQ��ǉ�
// �X�V��		�F2020/11/17		�_���[�W��ԂP�A�Q�̏�����ǉ�
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.h
// �T�v			�FVR���[�V�����̑Ή�
// �쐬��		�F19CU0236 �щ_��
// �쐬��		�F2020/08/28
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Smartphone.h"
#include "Components/ChildActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundClass.h"
#include "HeartBeatAppWidgetComponent.h"	// �S�����A�v���p�i�쐬�ҁF��K�j
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

	// =====  VR Motion �R���g���[���[ �֐�  (�쐬��:�щ_��) =====
	// Resets HMD orientation and position in VR
	void OnResetVR();

private:
	// �J����(Pitch)�̍X�V
	void UpdateCameraPitch(const float _deltaTime);

	// �J����(Yaw)�̍X�V
	void UpdateCameraYaw(const float _deltaTime);

	// �v���C���[�̈ړ�����
	void UpdatePlayerMove(const float _deltaTime);

	// �n�ʂ̂Ƃ̋����𑪂�v���C���[�̍�����ݒ�
	void SetEyeLevel(const float _deltaTime, const float _player_move_speed);

	// �v���C���[�̕��s�ɂ�鎋���̏c�h��
	float ReturnCameraVerticalShaking(const float _deltaTime, const float _player_move_speed);

	// ������炷
	void MakeFootstep(const float _deltaTime, const float _player_move_speed);

	// �A�C�e���`�F�b�N
	void CheckItem();

	// �x�N�g���̒�����Ԃ�
	float ReturnVector2DLength(const FVector2D* _pFvector2d);

	// �x�N�g���𐳋K������
	void NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d);

	// =====  VR Motion �R���g���[���[ �|�C���^�[�̊֐�  (�쐬��:�щ_��) =====
	void UpdateVRLaser();
	void CheckStandingVR();

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
	void PlayerStand();
	// �v���C���[�A�N�V�����F���Ⴊ��
	void PlayerSquat();

	// �v���C���[�A�N�V�����F�E���A���ׂ�A�쓮������
	void CheckToActor();

	// �X�}�z�֌W
	// �\���邩�ǂ����̃t���O(�쐬�ҁF����)
	void ChangeHaveSmartphoneFlag();

	// ���C�g�����邩�ǂ���(�쐬�ҁF����)
	void ChangeLightFlag()
	{
		lightFlag = !lightFlag;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Push Smartphone_Light");

	};
	// �V���b�^�[��؂邩�ǂ���(�쐬�ҁF����)
	void ChangeShutterFlag()
	{
		shatterFlag = !shatterFlag;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Push Smartphone_Shutter");
	};

public:
	UPROPERTY(EditAnywhere, Category = "Smart Phone App")
	bool isHeartBeatOn;			// �S�����A�v���̐؂�ւ�(�쐬�ҁF��K)
	// �S�����A�v���̐؂�ւ�(�쐬�ҁF��K)
	// Private->Public�ɕύX(UPROPETY�ϐ��錾��private�ɂ��Ȃ������ǂ�)
	void HeartBeatStatusSwitch()
	{
		// VR�R���g���[���ɑΉ����邽�ߒǉ�����(�쐬��:�щ_��)
		if (smartPhone_Mode_Num == 3)
		{
			isHeartBeatOn = !isHeartBeatOn;
			// �C���X�^���X�����邩�ǂ����̊m�F(�쐬�ҁF��K)
			if (UHeartBeatAppWidgetComponent::GetInstance())
			{
				UHeartBeatAppWidgetComponent::GetInstance()->SetAppStat(isHeartBeatOn);
			}
			UE_LOG(LogTemp, Log, TEXT("HeartBeat App Status Switched"));
		} // end if()

	}
	// Smartphone����Ăяo���֐�(isHaveSmartphoneFlag��Smartphone�ɑ���)(�쐬�ҁF����)
	bool GetisHaveSmartphoneFlag();

	// Smartphone����Ăяo���֐�(lightFlag��Smartphone�ɑ���)(�쐬�ҁF����)
	bool GetLightFlag();

	// Smartphone����Ăяo���֐�(shutterFlag��Smartphone�ɑ���)(�쐬�ҁF����)
	bool GetShatterFlag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
		bool in_the_locker_;	// ���b�J�[�ɓ��낤�Ƃ���`���S�ɏo��܂ł̃t���O(�쐬�ҁF����

	void SetInTheLocker(const bool flag);				// ��`��cpp�Ɉڂ�܂����@(�쐬��:�щ_��)

	// Phone�A�N�^�[���擾����֐��i�쐬�ҁF��K�j
	UFUNCTION(BlueprintGetter)
		AActor* GetPhoneActor() { return vr_Phone; };

public:
	UFUNCTION(BlueprintCallable, Category = "Return State")
		AItemBase* ReturnCheckingItem() const;

	UFUNCTION(BlueprintCallable, Category = "Return State")
		FString ReturnCheckingItemCommandName() const;

	unsigned char player_state;

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

	UPROPERTY(EditAnyWhere, Category = "Move")
		float player_footstep_span_;			// �v���C���[�̑����̊Ԋu(�v���C���[�̑��x / �Ԋu = 1�b�Ԃɖ鑫���̐�)

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMin;			// �J������Pitch�����ŏ��p�x

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMax;			// �J������Pitch�����ő�p�x

	UPROPERTY(EditAnywhere, Category = "Camera")
		class USceneComponent* m_pCameraBase;	// �J�����̌��_

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* m_pCamera;		// �J����

	UPROPERTY(EditAnywhere, Category = "Camera")
		class USpringArmComponent* m_pSpringArm;		// �X�v�����O�A�[��  (�쐬��:�щ_��)

	UPROPERTY(EditAnywhere)
		USoundClass* se_volume_can_change_;		// �e���ʉ��ɐݒ肵�Ă���T�E���h�N���X

	UPROPERTY(EditAnywhere)
		USoundBase* sound_player_footstep_;		// �v���C���[�̑���SE

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_eyeLevelWhenStanding;			// �����Ă���Ƃ��̖ڂ̍���

	UPROPERTY(EditAnywhere, Category = "Camera")
		float camera_shaking_value;				// ���s���̃J�����̗h��

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputPitch;				// Pitch���씽�]

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputYaw;					// Yaw���씽�]

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraRotateSpeed;				// �J�����̉�]�X�s�[�h

	UPROPERTY(EditAnywhere, Category = "Trace")
		float check_to_actor_trace_length_;

	UPROPERTY(Editanywhere, Category = "Trace")
		bool draw_debug_trace_;

	UPROPERTY(Editanywhere, Category = "Trace")
		FVector box_half_size_;

	EDrawDebugTrace::Type draw_debug_trace_type_;

	// �g���ĂȂ�
	bool isVRCamera;							// VR�J�������ǂ���

	float count_for_footstep_;					// �����̂��߂̃J�E���g

	float eyelevel_for_camera_shaking;			// �J�����̏c�h��̓x����

	bool can_make_footstep;						// �����������t���O

	bool can_player_control;					// �v���C���[�̑��삪�o���邩

	float m_playerMoveSpeed;					// �v���C���[�̈ړ����x

	int damage_count_;							// �G����U�����󂯂���(1��c���o�f�o�t�A2��c�G�o�f�o�t�A3��c���o�f�o�t�A4��c��)

	FVector2D m_playerMoveInput;				// �v���C���[�̈ړ����͗�
	FVector2D m_cameraRotateInput;				// �J�����̉�]���͗�

	AItemBase* m_pCheckingItem;					// �`�F�b�N���̃A�C�e��
	AItemBase* m_pPrevCheckItem;				// 1�t���[���O�Ƀ`�F�b�N���Ă����A�C�e��)

	//-----------------------------------------------------------------------------------------------------------------
	// ���o�f�o�t�̂��߂̃v���p�e�B�e���ʉ��̉��ʂɓK�p���ċ^���I�ɒ��o����܂����悤�ɂ݂���(�쐬�� ����I�l)
	//-----------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Hearing Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Sound Volume"))
		float se_volume_for_debuff_;

	//-----------------------------------------------------------------------------------------------------------------
	// ���o�f�o�t�̂��߂̃|�X�g�v���Z�X�v���p�e�B(�쐬�� ����I�l)
	//-----------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "2.0", DisplayName = "Color Saturation"))
		FVector4 saturation_for_debuff_;			// �ʓx(�O���[�X�P�[���ɂ��邽�߂ɂ�W��0��)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "2.0", DisplayName = "Color Contrast"))
		FVector4 contrast_for_debuff_;				// �R���g���X�g

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Vignette Intensity"))
		float vignette_intensity_for_debuff_;		// �r�l�b�g���ʋ��x(��ʎ��肪�Â��Ȃ�)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Grain Intensity"))
		float grain_intensity_for_debuff_;			// �O���C�����x(�m�C�Y)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Film Slope"))
		float film_slope_for_debuff_;

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "FilmToe"))
		float film_toe_for_debuff_;
	//-----------------------------------------------------------------------------------------------------------------

	// �R���X�g���N�^����
	bool isHaveSmartphoneFlag;		// �X�}�z���\���邩�ǂ���(�쐬�ҁF����)
	bool shatterFlag;				// �V���b�^�[��؂邩(�쐬�ҁF����)
	bool lightFlag;					// ���C�g�̐؂�ւ�(�쐬�ҁF����)


	// =====  VR Motion �R���g���[���[�@�v���p�e�B  by_Rin =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRMotion|Variables", meta = (AllowPrivateAccess = "true"))
		class AThrillerVR_MotionController* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRMotion|Variables", meta = (AllowPrivateAccess = "true"))
		class AThrillerVR_MotionController* RightController;

	FHitResult vr_HitResult;

	FString path = "Blueprint'/Game/Blueprints/BP_VRSmartPhone.BP_VRSmartPhone_C'";

	// VR's smart phone component
	UPROPERTY(EditAnywhere, Category = "VR_Phone")
		TSubclassOf<class AActor> bp_VRphone;

	// �R���X�g���N�^����
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPhoneActor, Category = "VR_Phone", meta = (AllowPrivateAccess = "true"))
		AActor* vr_Phone;


public: 
	// ===== �ړ��Ƃ��Ⴊ�ށ@�v���p�e�B  (�쐬��:�щ_��) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		bool m_isStanding;							// �����Ă��邩�ǂ����̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Walk ;					// �����Ă̈ړ����x

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Run;					// ����̈ړ����x

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Crouch;				// ���Ⴊ�ނ̈ړ����x

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float m_VRPlayersHeight;					// VR���[�h�̎������v���C���[�̗����Ă���̍���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		bool m_HeightisChecked;						// VR�̃v���C���[�g���m�F���܂�����

	// �R���X�g���N�^����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		bool vr_isVRmode;							// VR���[�h���ǂ����̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		bool vr_InCameraMode;						// VR�̃J�������[�h���ǂ����̃t���O

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		int holdingSmartphoneState;					// �X�}�z������O�Ɏ����Ă��邩?

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int vr_SmartPhone_Mission_Num;				// ���\������~�b�V�����i���o�[

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		int smartPhone_Mode_Num;					// �X�}�z�A�v�����[�h

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		FString vr_SmartPhone_Mission_Contents;		// ���\������~�b�V�����@�i���͎g���Ă��Ȃ��j

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR_Phone|Mission")
		int finished_MsiionID;						// �e�X�g�p�@�@�@���������~�b�V����ID�@�i�\���������Ȃ�ID������j

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStatus")
		TArray<int> missionStatus ;					// �~�b�V�����̊�����Ԃ��Ǘ�����array

	// 0:Insert, 1:Delete, 2:Update
	UFUNCTION(BlueprintCallable, Category = "VR_Phone|Mission")
		void UpdateTheMission(int _updateMode, int _missionID, bool& _hasUpdated);	// _missionID�Ƃ����~�b�V�����̃t���O�ƕ\������������

	// using for return bool. The calling  blueprint function can't return variable, so using players's variable to store the bool.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool missionTableHasUpdated;											// �X�}�z�̃~�b�V�������A�b�v�f�[�g����܂�����(Blueprint�p)

	UFUNCTION(BlueprintCallable, Category = "VR_Phone|Mission")
		int GetTheWideStringsByteLength(FString _inString, FText _inText);		// ��������Byte���J�E���g����

	bool isFound;		// �G�̍U���͈͓��ɓ�������(�쐬�ҁF����)

	UFUNCTION(BlueprintCallable, Category = "PlayerDamage")
		void SetIsFound(const bool _flag, FVector _enemy_location);			// ���������ꍇ��isFound��ς���֐�(�쐬�ҁF����)

	// �_���[�W���󂯂鎞��BP����Ă΂��֐�
	UFUNCTION(BlueprintCallable, Category = "PlayerDamage")
		void AttackFromEnemy();

	// �_���[�W��ԂP�ɂȂ������Ăяo���֐��BBlueprint�ɃI�[�o�[���C�h����i�쐬�ҁF��K�j
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void EarDamaged();

	// �_���[�W��ԂQ�ɂȂ������Ăяo���֐��BBlueprint�ɃI�[�o�[���C�h����i�쐬�ҁF��K�j
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void HandDamaged();

	// �_���[�W��ԂR�ɂȂ������Ăяo���֐��BBlueprint�ɃI�[�o�[���C�h����i�쐬�ҁF��K�j
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void EyeDamaged();
	
	FVector ReturnCameraLocation();

	FVector ReturnCameraForwardVector();

	void SetPlayerControlFlag(bool _flag) { can_player_control = _flag; }

	void Respawn();	// ���X�|�[������֐�(�쐬�ҁF����)

	//UFUNCTION(BlueprintCallable, Category = "PlayerMakeNoise")
};