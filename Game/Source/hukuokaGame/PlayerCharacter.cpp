//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�F�v���C���[�L�����𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �쐬��		�F19CU0217 ��K
// �X�V��		�F2020/08/18		VR�J�����̍쐬
// �X�V��		�F2020/10/09		�Q�[���I�[�o�[�ɑJ�ڂ̋@�\��ǉ�
// �X�V��		�F2020/10/14		���g�̐���̒ǉ�
// �X�V��		�F2020/11/04		�S�����A�v���̃A�N�V�����}�b�s���O��ǉ�
// �X�V��		�F2020/11/16		�_���[�W��ԂQ��ǉ�
// �X�V��		�F2020/11/17		�_���[�W��ԂP�A�Q�̏�����ǉ�
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�FVR���[�V�����̑Ή�
// �쐬��		�F19CU0236 �щ_�� 
// �쐬��		�F2020/08/28		VR���[�V�����R���g���[���[�̑Ή�
// �X�V��		�F2020/09/06		VR��ray�̍쐬
//				�F2020/09/19		VR�̃X�}�[�g�t�H���쐬
//				�F2020/10/20		Player�ړ��Ƃ��Ⴊ�ނ̒���
//								�X�v�����O�A�[���̒ǉ�
//				�F2020/10/28		VR�̈ړ��Ƃ��Ⴊ�ނ̒���
//				�F2020/11/05		PC�ŃX�}�z����O�Ɏ����@�ύX
//				�F2020/11/25		VR���[�h�̎��X�}�z�g����悤�ɂɐݒ肷��
//				�F2020/11/29		VR��CheckItem�ǉ�
//-------------------------------------------------------------------

#include "PlayerCharacter.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VirtualReality/ThrillerVR_MotionController.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
// �ȏ��6�_��VR�p�C���N���[�h (�쐬��:�щ_��)
#include "LevelSwitchHelper.h"	// �}�b�v�J�ڗp�N���X
#include "GameFramework/Actor.h" // makeNoise�p
#include "Containers/StringConv.h"	// ���������J�E���g�p


#define LASERLENGTH 250.0f		// VR�p LASER�̒���(��̒������\���邱��) (�쐬��:�щ_��)

// �R���X�g���N�^
APlayerCharacter::APlayerCharacter()
	: isHeartBeatOn(false)
	, in_the_locker_(false)
	, player_state(0)
	, m_isStanding(true)
	, m_playerThresholdToRun(1.0f)
	, m_playerRunSpeed(10.0f)
	, m_playerWalkSpeed(5.0f)
	, player_footstep_span_(200.0f)
	, m_cameraPitchLimitMin(-89.0f)
	, m_cameraPitchLimitMax(89.0f)
	, m_pCamera(NULL)
	, se_volume_can_change_(NULL)
	, sound_player_footstep_(NULL)
	, m_eyeLevelWhenStanding(170.0f)
	, camera_shaking_value(10.0f)
	, m_reverseInputPitch(false)
	, m_reverseInputYaw(false)
	, m_cameraRotateSpeed(100.0f)
	, check_to_actor_trace_length_(1300.0f)
	, draw_debug_trace_(false)
	, box_half_size_(FVector(50.f, 50.f, 50.f))
	, draw_debug_trace_type_(EDrawDebugTrace::None)
	, count_for_footstep_(0.0f)
	, eyelevel_for_camera_shaking(0.0f)
	, can_make_footstep(true)
	, can_player_move_control_(true)
	, can_player_camera_control_(true)
	, is_damaged_(false)
	, m_playerMoveSpeed(0.0f)
	, damage_count_(0)
	, m_playerMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_pCheckingItem(NULL)
	, m_pPrevCheckItem(NULL)
	, se_volume_for_debuff_(1.f)
	, saturation_for_debuff_(1.f, 1.f, 1.f, 0.f)
	, contrast_for_debuff_(1.f, 1.f, 1.f, 2.f)
	, vignette_intensity_for_debuff_(1.f)
	, grain_intensity_for_debuff_(0.5f)
	, film_slope_for_debuff_(1.f)
	, film_toe_for_debuff_(0.8f)
	, vr_HitResult(NULL)
	, m_VRPlayersHeight(175.0f)
	, m_HeightisChecked(false)
	, vr_InCameraMode(false)
	, holdingSmartphoneState(0)
	, vr_SmartPhone_Mission_Num(1)
	, finished_MsiionID(0)
	, missionTableHasUpdated(false)
	, isFound(false)
{
	// �e�B�b�N���Ăяo�����̃t���O
	PrimaryActorTick.bCanEverTick = true;

	// �f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// �X�v�����O�A�[���̃I�u�W�F�N�g�𐶐�
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));

	// �X�v�����O�A�[����RootComponent���A�^�b�`
	m_pSpringArm->SetupAttachment(RootComponent);

	// �J�����ƃv���C���[�̋���
	m_pSpringArm->TargetArmLength = 0.f;

	// �J�����̎q���W�����e�X�g���s������ݒ�
	m_pSpringArm->bDoCollisionTest = false;

	// �J�����Ǐ]���O���g������ݒ�
	m_pSpringArm->bEnableCameraLag = false;

	// �J�����Ǐ]���O�̑��x��ݒ�
	m_pSpringArm->CameraLagSpeed = 35.f;

	// �J�������_�̐���
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// �J�����𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// �J�������_�ɃX�v�����O�A�[�����A�^�b�`
	m_pCameraBase->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);

	// �J�������_�ɃJ�������A�^�b�`
	m_pCamera->SetupAttachment(m_pCameraBase);

	// �_���[�W���o�p���������_�J�[�u�擾
	const ConstructorHelpers::FObjectFinder<UCurveFloat> curve_for_damage_effect(TEXT("CurveFloat'/Game/Curve/DamageEffect.DamageEffect'"));

	damage_effect_timeline_ = FTimeline{};

	// �_���[�W���󂯂��ۂ̐Ԃ��Ȃ�G�t�F�N�g�X�V�����̃o�C���h
	FOnTimelineFloat timeline_update;
	timeline_update.BindUFunction(this, "TimelineUpdate");
	damage_effect_timeline_.AddInterpFloat(curve_for_damage_effect.Object, timeline_update);


	// �_���[�W���󂯂��ۂ̐Ԃ��Ȃ�G�t�F�N�g�I�������̃o�C���h
	FOnTimelineEvent timeline_finish;
	timeline_finish.BindUFunction(this, "TimelineFinish");
	damage_effect_timeline_.SetTimelineFinishedFunc(timeline_finish);
}

// �f�X�g���N�^
APlayerCharacter::~APlayerCharacter()
{
}

// ���s���Ɉ�x�Ă΂��
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GEngine->FixedFrameRate = 60.f;

	// ���ʉ��̉��ʂ�������(=1��)
	if (se_volume_can_change_ != NULL)	se_volume_can_change_->Properties.Volume = 1.f;

	// �{�b�N�X�g���[�X�̕`��ݒ�
	if (draw_debug_trace_)	draw_debug_trace_type_ = EDrawDebugTrace::ForOneFrame;

	// ===========  �v���C���[�ړ��A���Ⴊ�ޗp�̃v���p�e�B�ݒ�  (�쐬��:�щ_��) ===========
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;					// ���Ⴊ�ނ��\���܂�
	GetCharacterMovement()->CrouchedHalfHeight = 40.f;							// ���Ⴊ�ގ��̍���
	GetCharacterMovement()->bShrinkProxyCapsule = true;							// ���Ⴊ�ގ���collision�̕ύX���\���܂�(�K�v�Ȃ�����)
	GetCharacterMovement()->MaxAcceleration = 500.f;							// �v���C���[�ړ��̉����x

	// Epic Comment :D // Setup Player Height for various Platforms (PS4, Vive, Oculus)
	FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	if (DeviceName == "SteamVR" || DeviceName == "OculusHMD")
	{
		// Epic Comment :D // Windows (Oculus / Vive)
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	} // end if()
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Can't find VR Divice"));
	} // end else

	// ===========  VR Motion Controller's Spawn and Attach  (�쐬��:�щ_��) ===========
	// �����^�C�g����ʂɃ��[�h��I������Ȃ�Aif�̏�����ς��܂�
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// Epic Comment :D // Spawn and attach both motion controllers
		const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		// Epic Comment :D // "Hand" is available by checking "Expose on Spawn" in the variable on BP_MotionController.
		// SomWorks :D //  Expose on Spawn Variable parameter setup in c++ -> Use SpawnActorDeferred
		LeftController = GetWorld()->SpawnActorDeferred<AThrillerVR_MotionController>(AThrillerVR_MotionController::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (LeftController)
		{
			// SomWorks :D // ...setstuff here..then finish spawn..
			LeftController->Hand = EControllerHand::Left;
			LeftController->FinishSpawning(SpawnTransform); // UGameplayStatics::FinishSpawningActor(LeftController, SpawnTransform);
			LeftController->AttachToComponent(m_pCameraBase, AttachRules);
			LeftController->GetMotionController()->bDisableLowLatencyUpdate = true;

			/*
			bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// path�ɂ���N���X���擾
			if (bp_VRphone != nullptr)
			{
				vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VR�̃X�}�[�g�t�H����Actor�Ƃ��Đ�������

				vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
				vr_Phone->SetActorEnableCollision(false);

				if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
				{
					// VR�p�z�u
					// �X�}�z���̕����A��Ɋm�F���܂��� (��(���b�V���̏��������Ή���)Y X Z)
					vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ��1

					// vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||

					//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

					//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ��display
																									//   ||
					vr_Phone->SetActorRelativeLocation(FVector(200, 0, 10));

					// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
				} // end if
				else
				{
					// PC�m�F�p�z�u
					vr_Phone->SetActorRelativeRotation(FRotator(-90.f, -180.f, 180.f));
					vr_Phone->SetActorRelativeLocation(FVector(400, 0, 70));
				} // end else

				// �m�F�p
				// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetName()));

			} // end if()
			*/
		} // end if()

		RightController = GetWorld()->SpawnActorDeferred<AThrillerVR_MotionController>(AThrillerVR_MotionController::StaticClass(), SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (RightController)
		{
			RightController->Hand = EControllerHand::Right;
			RightController->FinishSpawning(SpawnTransform);
			RightController->AttachToComponent(m_pCameraBase, AttachRules);
			RightController->GetMotionController()->bDisableLowLatencyUpdate = true;
		} // end if()
	} // end if()
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Is Not VR Mode"));
	} // end else

	// ���pPC�X�}�z
	// Epic Comment :D // Spawn and attach both motion controllers
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// path�ɂ���N���X���擾
	if (bp_VRphone != nullptr)
	{
		vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VR�̃X�}�[�g�t�H����Actor�Ƃ��Đ�������

		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
		{
			vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
			vr_Phone->SetActorEnableCollision(false);

			// VR�p�z�u
			// �X�}�z���̕����A��Ɋm�F���܂��� (��(���b�V���̏��������Ή���)Y X Z)
			vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ��

			// vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||

			//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

			//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ��display
																							//   ||
			vr_Phone->SetActorRelativeLocation(FVector(100, 0, 10));

			// VR�X�}�z�̃T�C�Y
			vr_Phone->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));

			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
			
			holdingSmartphoneState = 1;
		} // end if
		else
		{
			vr_Phone->AttachToComponent(m_pCamera, AttachRules);
			vr_Phone->SetActorEnableCollision(false);

			// PC�m�F�p�z�u
			vr_Phone->SetActorRelativeRotation(FRotator(-90.f, -180.f, 180.f));
			vr_Phone->SetActorRelativeLocation(FVector(200.f, -150.f, -50.f));

			// PC�X�}�z�̃T�C�Y
			vr_Phone->SetActorScale3D(FVector(0.4f, 0.4f, 0.4f));

			vr_Phone->SetActorHiddenInGame(true);

		} // end else

		// �m�F�p
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetName()));

	} // end if()

}

// ���t���[���Ă΂��
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �J����(Pitch)�̍X�V
	UpdateCameraPitch(DeltaTime);

	// �J����(Yaw)�̍X�V
	UpdateCameraYaw(DeltaTime);

	// �v���C���[�L�����N�^�[�̍X�V
	UpdatePlayerMove(DeltaTime);

	// ===========  VR Motion Controller's Laser Update (�쐬��:�щ_��) ===========
	// ����VR���[�h?
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// �A�C�e���̃`�F�b�N
		CheckItem();
	}
	// ===========  VR Motion Controller's Laser Update (�쐬��:�щ_��) ===========
	else 
	{
		// VR �R���g���[���[ �|�C���^�[�̍X�V
		// VR�̃A�C�e���̃`�F�b�N
		UpdateVRLaser();
	} // end if()

	// �_���[�W��ԂȂ�TimeLine�X�V
	if (is_damaged_)	damage_effect_timeline_.TickTimeline(DeltaTime);
}

// �e���͊֌W���\�b�h�Ƃ̃o�C���h����
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// �v���C���[�̈ړ�
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::PlayerMoveX);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::PlayerMoveY);

	// �J�����̉�]
	InputComponent->BindAxis("CameraPitch", this, &APlayerCharacter::CameraRotatePitch);
	InputComponent->BindAxis("CameraYaw", this, &APlayerCharacter::CameraRotateYaw);

	// �����オ��A���Ⴊ��
	InputComponent->BindAction("PlayerSquat", IE_Pressed, this, &APlayerCharacter::PlayerSquat);
	InputComponent->BindAction("PlayerSquat", IE_Released, this, &APlayerCharacter::PlayerStand);

	// �v���C���[�A�N�V�����F�E���A���ׂ�A�쓮������
	InputComponent->BindAction("PickUpandCheck", IE_Released, this, &APlayerCharacter::CheckToActor);

	// �X�}�z���\����E�\��������(�쐬�ҁF����)�@���͎g���Ă��� (�쐬��:�щ_��)
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);

	// �S�����A�v���̐؂�ւ�
	// VR�R���g���[���ɑΉ����邽�ߓ���ɂȂ���(�쐬��:�щ_��)
	InputComponent->BindAction("UseSmartPhone", IE_Pressed, this, &APlayerCharacter::HeartBeatStatusSwitch);
}

// �J����(Pitch)�̍X�V
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
	// ����s�Ȃ�return
	if (!can_player_camera_control_)	return;

	if (m_pCamera != NULL)
	{
		// ���݂̃J�����̉�]�����擾
		FRotator newRotationCamera = m_pCamera->GetRelativeRotation();

		// Pitch(�J��������]������)
		newRotationCamera.Pitch = FMath::Clamp((newRotationCamera.Pitch - (m_cameraRotateInput.Y * m_cameraRotateSpeed * _deltaTime)), m_cameraPitchLimitMin, m_cameraPitchLimitMax);

		// �J�����ɉ�]����ݒ�
		m_pCamera->SetRelativeRotation(newRotationCamera);
	}
}

// �J����(Yaw)�̍X�V
void APlayerCharacter::UpdateCameraYaw(const float _deltaTime)
{
	// ����s�Ȃ�return
	if (!can_player_camera_control_)	return;

	// ���݂̃v���C���[�̉�]�����擾
	FRotator newRotationPlayer = GetActorRotation();
	
	// VR's ��] (�쐬��:�щ_��)
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// �������Ȃ�
			
		/*
		// Rotation�m�F�p
		FRotator HMDRotation;
		FVector HMDLocation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::SanitizeFloat(HMDRotation.Quaternion().Z));
		*/

	} // end if()
	else
	{
		// Yaw(�v���C���[����]������)		masui
		newRotationPlayer.Yaw += m_cameraRotateInput.X * m_cameraRotateSpeed * _deltaTime;
	}

	// �v���C���[�ɉ�]����ݒ�
	SetActorRotation(newRotationPlayer);
}

// �v���C���[�̈ړ�����
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// ����s�Ȃ�return
	if (!can_player_move_control_ || in_the_locker_)	return;

	// �x�N�g���̒������擾
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// �ړ��ʂ�����		(�C����:�щ_��)(12/06 �d�l���ʂ�̋��������Ă��Ȃ����ߍďC�� �C����:����I�l)
	/*
	// unreal engine's bug, dont use it. Using BluePrint Now.
		GetCharacterMovement()->UnCrouch(true);
	*/

	// ����
	if (vectorLength >= m_playerThresholdToRun)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
		m_playerMoveSpeed = m_playerRunSpeed;
	}
	// ����
	else if (vectorLength > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
		m_playerMoveSpeed = m_playerWalkSpeed;
	}
	// �~�܂�
	else
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
		m_playerMoveSpeed = 0.0f;
	}

	// �����Ă��鎞�̑��x�ݒ�
	GetCharacterMovement()->MaxWalkSpeed = m_playerMoveSpeed;

	// ���Ⴊ��ł����ꍇ�ݒ肵�����x��1/2��
	if (m_isStanding == false && (GetCharacterMovement()->IsCrouching() == true))
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Squat"));
		m_playerMoveSpeed /= 2.0f;

		// ���Ⴊ��ł��鎞�̑��x�ݒ�
		// GetCharacterMovement()->MaxWalkSpeedCrouched = m_playerMoveSpeed;

		/*
		// unreal engine's bug, dont use it. Using BluePrint Now.
		if (GetCharacterMovement()->IsCrouching() == false)
		{
			GetCharacterMovement()->Crouch(true);
		} // end if
		*/
	}

	// �x�N�g���̐��K��
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// �v���C���[�̈ړ����x�m�F�p
	GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Purple, FString::SanitizeFloat(GetCharacterMovement()->Velocity.Size()));

	// �n�ʂƂ̋����𑪂�v���C���[�̍�����ݒ�
	SetEyeLevel(_deltaTime, (m_playerMoveSpeed * _deltaTime));

	// VR���̈ړ� (�쐬��:�щ_��)
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// VR's HMD rotation.z maybe is Radian, its between -1 ~ 1
		// so use VR camera's Rotation

		AddMovementInput(m_pCamera->GetForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(m_pCamera->GetRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));

	} // end if()
	// PC���̈ړ�
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("=== %s"), *GetActorForwardVector().ToString()));

		// PC���̈ړ�
		AddMovementInput(GetActorForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(GetActorRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));
	} // end else
}

void APlayerCharacter::PlayerStand() {
	// C++�̂��Ⴊ�ނ��g���Ȃ�����, �R�����g�A�E�g���܂����iBluePrint�ɐݒ肵�Ă���j  (�쐬��:�щ_��)
	// m_isStanding = true;
}

// �v���C���[�A�N�V�����F���Ⴊ��
void APlayerCharacter::PlayerSquat() {
	// C++�̂��Ⴊ�ނ��g���Ȃ�����, �R�����g�A�E�g���܂����iBluePrint�ɐݒ肵�Ă���j	(�쐬��:�щ_��)
	// m_isStanding = false;
}

// �n�ʂƂ̋����𑪂�v���C���[�̍�����ݒ�
void APlayerCharacter::SetEyeLevel(const float _deltaTime, const float _player_move_speed)
{
	// �v���C���[�̕��s�ɂ�鎋���̏c�h��
	eyelevel_for_camera_shaking = ReturnCameraVerticalShaking(_deltaTime, _player_move_speed);

	// �����Ă���΂��̂܂܂��Ⴊ��ł���΃A�C���x����1/4�ɂ��č��W�Z�b�g
	// ���Ⴊ�ނƍ��킹�Ă̏�����������	(�쐬��:�щ_��)
	if (m_isStanding && (GetCharacterMovement()->IsCrouching() == false))
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Stand eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));
		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f,  eyelevel_for_camera_shaking));

		// PC ���Ⴊ�ނ̎��@�X�}�z�ʒu�̒���
		if (holdingSmartphoneState == 1)
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
			{
				vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, -2.f));			// PC�p�̃X�}�z�z�u
			
			} // end if()
		} // end if()
		
	}
	else
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Crouch eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, eyelevel_for_camera_shaking));
		
		// PC ���Ⴊ�ނ̎��@�X�}�z�ʒu�̒���
		if (holdingSmartphoneState == 1)
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
			{
				vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, 0.f));			// PC�p�̃X�}�z�z�u
			} // end if()
		} // end if()
		
	}
}

// �v���C���[�̕��s�ɂ�鎋���̏c�h��
float APlayerCharacter::ReturnCameraVerticalShaking(const float _deltaTime, const float _player_move_speed)
{
	// �ړ����Ă��Ȃ���΃f�t�H���g�̖ڂ̍����ֈړ�����
	if (_player_move_speed <= 0.0f)
	{
		if (count_for_footstep_ < player_footstep_span_)
		{
			count_for_footstep_ -= player_footstep_span_ * 0.1;
		}
		else
		{
			count_for_footstep_ += player_footstep_span_ * 0.1;
		}
	}
	// �ړ����Ă���Έړ��ʂ����Z
	else
	{
		count_for_footstep_ += _player_move_speed;
	}
	
	if (count_for_footstep_ >= player_footstep_span_ && can_make_footstep)
	{
		// �����𗧂Ă�A���t���O������
		MakeFootstep(_deltaTime, _player_move_speed);
		can_make_footstep = false;
	}

	// �͈͊O�Ȃ烊�Z�b�g�A�������t���O�𗧂Ă�
	if (count_for_footstep_ >= player_footstep_span_ * 2.0f || count_for_footstep_ < 0.0f)
	{
		count_for_footstep_ = 0.0f;
		can_make_footstep = true;
	}

	float eyelevel = count_for_footstep_ * (180.0f / player_footstep_span_);
	eyelevel = FMath::DegreesToRadians(eyelevel);
	eyelevel = FMath::Cos(eyelevel);

	// �␳
	eyelevel += -1.0f;	// 0�`-2
	eyelevel *= 0.5f;	// 0�`-1
	eyelevel *= camera_shaking_value;	// 0�`camera_shaking_value

	return eyelevel;
}

// ������炷
void APlayerCharacter::MakeFootstep(const float _deltaTime, const float _player_move_speed)
{
	// ���ʒ���
	float volume = _player_move_speed / (m_playerRunSpeed * _deltaTime);
	
	// �Đ�
	if (sound_player_footstep_ != NULL)UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_player_footstep_, GetActorLocation(), volume, 1.0f, 0.0f);

	MakeNoise(volume, this, this->GetActorLocation(), 1.0f);
}

void APlayerCharacter::CheckItem()
{
	// ����s�Ȃ�\������Ă���R�}���h�A�C�R�����\���ɂ��Areturn
	if (!can_player_move_control_ || isFound || in_the_locker_)
	{
		if (m_pPrevCheckItem != NULL)
		{
			// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
			OnItemCheckEndEventDispatcher.Broadcast();
			m_pPrevCheckItem->SetOutline(false);
		}
		return;
	}

	// �g���[�X�ɕK�v�ȕϐ���錾
	FHitResult outHit;

	// ���݈ʒu���擾
	FVector start = m_pCamera->GetComponentLocation() + (m_pCamera->GetForwardVector() * 40.f);
	FVector end = start + (m_pCamera->GetForwardVector() * check_to_actor_trace_length_);

	// 1�t���[���O�̃A�C�e���̏����ڂ�
	m_pPrevCheckItem = m_pCheckingItem;

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);

	// ���C���g���[�X����{�b�N�X�g���[�X�ɕύX(11/20 ����)
	if(UKismetSystemLibrary::BoxTraceSingle(this, start, end, box_half_size_, GetActorRotation(), TraceTypeQuery3, false, actors_to_ignore, draw_debug_trace_type_, outHit, true, FLinearColor::Red, FLinearColor::Green, 1.0f))
	{
		// �A�C�e����{�N���X�ɃL���X�g
		m_pCheckingItem = Cast<AItemBase>(outHit.GetActor());

		if (m_pCheckingItem != NULL)
		{
			// 1�t���[���O�̃A�C�e���ƈႤ�Ȃ�X�V
			if (m_pCheckingItem != m_pPrevCheckItem)
			{
				// �O�t���[���ŗL���ȃI�u�W�F�N�g���`�F�b�N���Ă�����
				if (m_pPrevCheckItem != NULL)
				{
					// �O�t���[���Ń`�F�b�N���Ă����I�u�W�F�N�g�̔�`�F�b�N�𖳌���
					m_pPrevCheckItem->m_isChecked = false;

					// ���g���\���ɂ���	by	��K
					m_pPrevCheckItem->SetOutline(false);

					// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
					OnItemCheckEndEventDispatcher.Broadcast();
				}
				// �V�����`�F�b�N�����I�u�W�F�N�g�̔�`�F�b�N��L����
				m_pCheckingItem->m_isChecked = true;

				// ���g��\���ɂ���		by	��K
				m_pCheckingItem->SetOutline(true);

				// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�ɒǉ�)
				OnItemCheckBeginEventDispatcher.Broadcast();
			}
		}
		else
		{
			if (m_pPrevCheckItem != NULL)
			{
				// �O�t���[���Ń`�F�b�N���Ă����I�u�W�F�N�g�̔�`�F�b�N�𖳌���
				m_pPrevCheckItem->m_isChecked = false;

				// ���g���\���ɂ���	by	��K
				m_pPrevCheckItem->SetOutline(false);

				// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
				OnItemCheckEndEventDispatcher.Broadcast();
			}
		}
	}
	else
	{
		// �O�t���[���ł͌��m���Ă����ꍇ���̃A�C�e���̐ڐG�t���O��������
		if (m_pPrevCheckItem != NULL)
		{
			m_pPrevCheckItem->m_isChecked = false;

			// ���g���\���ɂ���	by	��K
			m_pPrevCheckItem->SetOutline(false);

			// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
			OnItemCheckEndEventDispatcher.Broadcast();
		}
		m_pCheckingItem = NULL;
	}
}

// �_���[�W���󂯂��ۂ̐Ԃ��Ȃ�G�t�F�N�g�X�V����
void APlayerCharacter::TimelineUpdate(float value)
{
	m_pCamera->PostProcessSettings.SceneColorTint = FVector(1.f, 1.f - value, 1.f - value);
	m_pCamera->PostProcessSettings.VignetteIntensity = value;
}

// �_���[�W���󂯂��ۂ̐Ԃ��Ȃ�G�t�F�N�g�I������
void APlayerCharacter::TimelineFinish()
{
	m_pCamera->PostProcessSettings.SceneColorTint = FVector(1.f, 1.f, 1.f);
	m_pCamera->PostProcessSettings.VignetteIntensity = 0.f;

	is_damaged_ = false;
}

// �x�N�g���̒�����Ԃ�
float APlayerCharacter::ReturnVector2DLength(const FVector2D* _pFvector2d)
{
	return FMath::Sqrt((_pFvector2d->X * _pFvector2d->X) + (_pFvector2d->Y * _pFvector2d->Y));
}

// �x�N�g���𐳋K������
void APlayerCharacter::NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d)
{
	float multipleNum = 1.0f / _vectorLength;

	_pFvector2d->X *= multipleNum;
	_pFvector2d->Y *= multipleNum;
}

// ���̓o�C���h
// �J������]�FPitch(Y��)
void APlayerCharacter::CameraRotatePitch(float _axisValue)
{
	// Pitch�̑��씽�]�t���O�������Ă����甽�]
	if (m_reverseInputPitch) m_cameraRotateInput.Y = _axisValue * -1.0f;
	// �~��Ă����炻�̂܂܂̓��͒l
	else                     m_cameraRotateInput.Y = _axisValue;
}
// �J������]�FYaw(Z��)
void APlayerCharacter::CameraRotateYaw(float _axisValue)
{
	// Pitch�̑��씽�]�t���O�������Ă����甽�]
	if (m_reverseInputYaw) m_cameraRotateInput.X = _axisValue * -1.0f;
	// �~��Ă����炻�̂܂܂̓��͒l
	else                   m_cameraRotateInput.X = _axisValue;
}

// �v���C���[�ړ��F�ړ�X������(�c)
void APlayerCharacter::PlayerMoveX(float _axisValue)
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Purple, TEXT("  X  "));

	m_playerMoveInput.X = _axisValue;
}
// �v���C���[�ړ��F�ړ�Y������(��)
void APlayerCharacter::PlayerMoveY(float _axisValue)
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Purple, TEXT("  Y  "));
	m_playerMoveInput.Y = _axisValue;
}
// �v���C���[�A�N�V�����F�E���A���ׂ�A�쓮������
void APlayerCharacter::CheckToActor()
{
	if (m_pCheckingItem != NULL)
	{
		m_pCheckingItem->CheckedByPlayer();
	}
}

void APlayerCharacter::AttackFromEnemy()
{
	++damage_count_;

	is_damaged_ = true;
	damage_effect_timeline_.PlayFromStart();

	switch (damage_count_)
	{
	case 1:
		// �_���[�W��ԂQ�ɂȂ�i�쐬�ҁF��K�j
		EarDamaged();

		// ��_���[�W1���(���o�f�o�t/���ʉ��̃T�E���h�N���X��ʂ��ĉ��ʂ̐ݒ�)
		if (se_volume_can_change_ != NULL)	se_volume_can_change_->Properties.Volume = se_volume_for_debuff_;
		else								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SE_VolumeCanChange is not set.");
		break;
	case 2:
		// �_���[�W��ԂQ�ɂȂ�i�쐬�ҁF��K�j
		HandDamaged();
		break;
	case 3:
		// �_���[�W��ԂR�ɂȂ�i�쐬�ҁF��K�j
		EyeDamaged();

		// ��_���[�W3���(���o�f�o�t/�|�X�g�v���Z�X�̒l�ݒ�)
		m_pCamera->PostProcessSettings.ColorSaturation = saturation_for_debuff_;
		m_pCamera->PostProcessSettings.ColorContrast = contrast_for_debuff_;
		m_pCamera->PostProcessSettings.VignetteIntensity = vignette_intensity_for_debuff_;
		m_pCamera->PostProcessSettings.GrainIntensity = grain_intensity_for_debuff_;
		m_pCamera->PostProcessSettings.FilmSlope = film_slope_for_debuff_;
		m_pCamera->PostProcessSettings.FilmToe = film_toe_for_debuff_;
		break;
	case 4:
		// ��_���[�W4���(��:���X�|�[��)
		Respawn();
		break;
	default:
		break;
	}
}

// �G�l�~�[�̍U���͈͂ɓ������ۂ̏��� (�ǋL�� ����)
void APlayerCharacter::SetIsFound(const bool _flag, const FVector _enemy_location)
{
	isFound = _flag;

	if (isFound)
	{
		can_player_move_control_ = false;

		if (damage_count_ == 3)
		{
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), (_enemy_location + FVector(0.f, 0.f, 50.f))));
			can_player_camera_control_ = false;
		}
	}
	else
	{
		can_player_move_control_ = true;
		can_player_camera_control_ = true;
	}
}

// PC�ŁA�X�}�z����O�Ɏ����Ă��邩  (�쐬��:�щ_��)
void APlayerCharacter::ChangeHaveSmartphoneFlag()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		if (in_the_locker_ == false)
		{
			// �X�}�z���|�P�b�g�ɂ��܂�
			if (holdingSmartphoneState == 2)
			{
				holdingSmartphoneState = 0;
				vr_Phone->SetActorHiddenInGame(true);

				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Is Not Holding Smartphone");
			} // end if()
			// �X�}�z��ڑO�Ɏ���
			else if (holdingSmartphoneState == 1)
			{
				holdingSmartphoneState = 2;
				vr_Phone->SetActorRelativeLocation(FVector(10.f, 0.f, 0.f));
				vr_Phone->SetActorHiddenInGame(false);
				vr_Phone->SetActorScale3D(FVector(0.06f, 0.06f, 0.06f));						// PC�X�}�z�̃T�C�Y

				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Looking at Smartphone");
			} // end else
			// �X�}�z����O�Ɏ���
			else
			{
				holdingSmartphoneState = 1;
				vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, -2.f));
				vr_Phone->SetActorHiddenInGame(false);
				vr_Phone->SetActorScale3D(FVector(0.02f, 0.02f, 0.02f));						// PC�X�}�z�̃T�C�Y

				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Is Holding Smartphone");
			} // end else
		} // end if()
		else
		{
			holdingSmartphoneState = 0;
			vr_Phone->SetActorHiddenInGame(true);

			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Is Not Holding Smartphone");
		} // end else

	} // end if()

} // ChangeHaveSmartphoneFlag()

AItemBase* APlayerCharacter::ReturnCheckingItem() const
{
	return m_pCheckingItem;
}

FString APlayerCharacter::ReturnCheckingItemCommandName() const
{
	FString command_name = "None";

	if (m_pCheckingItem != NULL)
	{
		command_name = m_pCheckingItem->m_commandName;
	}

	return command_name;
}

FVector APlayerCharacter::ReturnCameraForwardVector()
{ 
	FVector forward_vector = FVector::ZeroVector;

	if (m_pCamera != NULL)	forward_vector = m_pCamera->GetForwardVector();

	return forward_vector;
}

// _missionID�Ƃ����~�b�V�����̃t���O�ƕ\������������	(�쐬��:�щ_��)
// utility��Z�[�u�f�[�^�̃~�b�V�����t���O��������������ŕ��������ł��B
// _isDelete��true�̎��~�b�V��������ʂ���폜����B
// false�̎��~�b�V������ǉ�����
void APlayerCharacter::UpdateTheMission(int _updateMode, int _missionID, bool& _hasUpdated)
{
	// CurrentMissionUpdate(1, 1);
	FString FuncName_and_Solution = FString::Printf(TEXT("CurrentMissionUpdate "));
	FOutputDeviceNull ar;

	if (_updateMode == 0)
	{
		FuncName_and_Solution += FString::Printf(TEXT("0 "));
	} // end if()
	if (_updateMode == 1)
	{
		FuncName_and_Solution += FString::Printf(TEXT("1 "));
	} // end if()
	else if (_updateMode == 2)
	{
		FuncName_and_Solution += FString::Printf(TEXT("2 "));
	} // end if()
	else
	{
		UE_LOG(LogClass, Log, TEXT("(PlayerCharacter.cpp)(UpdateTheMission) : The _updateMode number got error."));
	} // end else

	FuncName_and_Solution += FString::FromInt(_missionID);
	vr_Phone->CallFunctionByNameWithArguments(*FuncName_and_Solution, ar, NULL, true);
	

	_hasUpdated = missionTableHasUpdated;

	// vr_Phone->CallFunctionByNameWithArguments(TEXT("CurrentMissionUpdate 1 1"), ar, NULL, true);

} // void UpdateTheMission()

// ���������C�hBYTE�ɕϊ�	(�쐬��:�щ_��)
int APlayerCharacter::GetTheWideStringsByteLength(FString _inString, FText _inText)
{
	int length_byByte = 0;

	// _inString = _inText.ToString();
	FTCHARToUTF8 ConvertedString(*_inString);
	//(uint8*)Converted.Get(), Converted.Length()

	length_byByte = ConvertedString.Length();
	// length_byByte = _inString.CountBytes() ;

	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::FromInt(length_byByte));
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("=== %s"), *GetActorForwardVector().ToString()));

	return length_byByte;
}  // GetTheWideStringsByteLength()


// �_���[�W��Ԃ̏����iC++���j�i�쐬�ҁF��K�j
void APlayerCharacter::EarDamaged_Implementation()
{
}
void APlayerCharacter::HandDamaged_Implementation()
{
}
void APlayerCharacter::EyeDamaged_Implementation()
{
}

// �J�����̍��W��Ԃ�
FVector APlayerCharacter::ReturnCameraLocation()
{
	FVector camera_location = FVector::ZeroVector;

	if (m_pCamera != NULL)	camera_location = m_pCamera->GetComponentLocation();

	return camera_location;
}

// �v���C���[�A�N�V�����F�X�}�z�̃V���b�^�[�t���O��ύX(�쐬�ҁF����)
bool APlayerCharacter::GetShatterFlag()
{
	return shatterFlag;
}

// ���b�J�[�ɂ��邩�ǂ����̃t���O�Z�b�g�֐�(�����ɂ����Ɠ��낤�Ƃ���Ƃ��납�犮�S�ɏo��܂�)(�쐬�ҁF����)
void APlayerCharacter::SetInTheLocker(const bool flag)
{
	in_the_locker_ = flag;
	// �X�}�z���g���Ȃ�����ǉ�(�쐬��:�щ_��)
	if (in_the_locker_ == true && holdingSmartphoneState != 0)
	{
		ChangeHaveSmartphoneFlag();
	} // end if()

}

// �v���C���[�A�N�V�����F�X�}�z�̃��C�g�̃t���O��ύX(�쐬�ҁF����)
bool APlayerCharacter::GetLightFlag()
{
	return lightFlag;
}

// �v���C���[�A�N�V�����F�X�}�z���\�����Ԃ̃t���O��ύX(�쐬�ҁF����)
bool APlayerCharacter::GetisHaveSmartphoneFlag()
{
	return isHaveSmartphoneFlag;
}


// ===========  VR HMD�̃��Z�b�g  (�쐬��:�щ_��) ===========
void APlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
} // void APlayerCharacter::OnResetVR()

// =====  VR Motion �R���g���[���[ �|�C���^�[�̊֐�  (�쐬��:�щ_��) =====
void APlayerCharacter::UpdateVRLaser()
{
	// ===============================================================
	// 11/29	VR CheckItem�ǉ�
	// 12/01	�g���[�X�^�C�v�ύX
	// ===============================================================

	FVector StartPoint = FVector::ZeroVector;			// Laser�̏����ʒu
	FVector FowardActor = FVector::ZeroVector;			// ���[�V�����R���g���[���[�̑O����

	// VR's laser start point from right controller
	if (RightController)
	{
		// StartPoint = RightController->ActorToWorld().GetLocation();

		StartPoint = RightController->GetMotionController()->K2_GetComponentLocation();
		FowardActor = RightController->GetRootComponent()->GetChildComponent(0)->GetForwardVector();
		StartPoint = ((FowardActor * 30.f) + StartPoint);

	} // end if()
	else
	{
		StartPoint = m_pCamera->GetComponentLocation();
		FowardActor = m_pCamera->GetForwardVector();
	} // end else

	// for checking the start point & Foward Vector
	// UE_LOG(LogClass, Log, TEXT("======: %s"), *StartPoint.ToString());
	// UE_LOG(LogClass, Log, TEXT("=====: %s"), *FowardActor.ToString());

	FVector EndPoint = ((FowardActor * LASERLENGTH) + StartPoint);				// Laser�̏I�_

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// �e�X�g�p�|�C���^�[
	// DrawDebugLine (GetWorld (), StartPoint, EndPoint, FColor::Red, false, 1.0f);

	ULineBatchComponent* const LineBatcher = GetWorld()->PersistentLineBatcher;


	// ����s�Ȃ�\������Ă���R�}���h�A�C�R�����\���ɂ��Areturn
	if (!can_player_move_control_ || isFound || in_the_locker_)
	{
		if (m_pPrevCheckItem != NULL)
		{
			// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
			OnItemCheckEndEventDispatcher.Broadcast();
			m_pPrevCheckItem->SetOutline(false);
		}
		return;
	}

	// 1�t���[���O�̃A�C�e���̏����ڂ�
	m_pPrevCheckItem = m_pCheckingItem;

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);
	actors_to_ignore.Add(LeftController);
	actors_to_ignore.Add(RightController);
	actors_to_ignore.Add(vr_Phone);


	// Laser���ŏ��ɓ��������̂��̂��@vr_HitResult�@�ɔ��f����
	// if (GetWorld()->LineTraceSingleByChannel(vr_HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
	if (UKismetSystemLibrary::BoxTraceSingleByProfile(RightController, StartPoint, EndPoint, (box_half_size_/2), RightController->GetMotionController()->K2_GetComponentRotation(), FName("RightContollerCheckCollision"), false, actors_to_ignore, EDrawDebugTrace::ForOneFrame, vr_HitResult, true,FLinearColor::Green,FLinearColor::Red, 0.1f))
	{

		// �A�C�e����{�N���X�ɃL���X�g
		m_pCheckingItem = Cast<AItemBase>(vr_HitResult.GetActor());

		if (m_pCheckingItem != NULL)
		{
			// 1�t���[���O�̃A�C�e���ƈႤ�Ȃ�X�V
			if (m_pCheckingItem != m_pPrevCheckItem)
			{
				// �O�t���[���ŗL���ȃI�u�W�F�N�g���`�F�b�N���Ă�����
				if (m_pPrevCheckItem != NULL)
				{
					// �O�t���[���Ń`�F�b�N���Ă����I�u�W�F�N�g�̔�`�F�b�N�𖳌���
					m_pPrevCheckItem->m_isChecked = false;

					// ���g���\���ɂ���	by	��K
					m_pPrevCheckItem->SetOutline(false);

					// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
					OnItemCheckEndEventDispatcher.Broadcast();
				}
				// �V�����`�F�b�N�����I�u�W�F�N�g�̔�`�F�b�N��L����
				m_pCheckingItem->m_isChecked = true;

				// ���g��\���ɂ���		by	��K
				m_pCheckingItem->SetOutline(true);

				// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�ɒǉ�)
				OnItemCheckBeginEventDispatcher.Broadcast();
			}


			if (vr_InCameraMode == false)
			{
				LineBatcher->DrawLine(StartPoint, EndPoint, FLinearColor::Red, 10, 0.f, 1.f);
			} // end if()
			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("RayHit: %s"), *HitResult.Actor->GetName()));

		}
		else
		{
			if (m_pPrevCheckItem != NULL)
			{
				// �O�t���[���Ń`�F�b�N���Ă����I�u�W�F�N�g�̔�`�F�b�N�𖳌���
				m_pPrevCheckItem->m_isChecked = false;

				// ���g���\���ɂ���	by	��K
				m_pPrevCheckItem->SetOutline(false);

				// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
				OnItemCheckEndEventDispatcher.Broadcast();
			}
		}

	} // end if()

	else
	{
		if (vr_InCameraMode == false)
		{
			LineBatcher->DrawLine(StartPoint, EndPoint, FLinearColor::Green, 10, 0.f, 1.f);
		} // end if()


		// �O�t���[���ł͌��m���Ă����ꍇ���̃A�C�e���̐ڐG�t���O��������
		if (m_pPrevCheckItem != NULL)
		{
			m_pPrevCheckItem->m_isChecked = false;

			// ���g���\���ɂ���	by	��K
			m_pPrevCheckItem->SetOutline(false);

			// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
			OnItemCheckEndEventDispatcher.Broadcast();
		}
		m_pCheckingItem = NULL;

	} // end else

} // APlayerCharacter::UpdateVRLaser()

// ���X�|�[���֐�(�쐬�ҁF����)
void APlayerCharacter::Respawn()
{
	//--- �Q�[���I�[�o�[�ɑJ�ڂ̒ǉ� by ��K
	LevelSwitchHelper::OpenGameOverLevelMap(this);
}

// C++�̂��Ⴊ�ނ��g���Ȃ�����
// CheckStandingVR��������
// (�쐬��:�щ_��)
/*
void APlayerCharacter::CheckStandingVR()
{
	FRotator checkHMDRotation;
	FVector checkHMDLocation;

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		if (m_HeightisChecked == false)
		{

			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(checkHMDRotation, checkHMDLocation);

			m_VRPlayersHeight = checkHMDLocation.Z;

			m_HeightisChecked = true;
		} // end if()

		if (m_HeightisChecked == true)
		{
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(checkHMDRotation, checkHMDLocation);

			if (checkHMDLocation.Z < (m_VRPlayersHeight * 0.7f))
			{
			} // end if()
			else
			{
			} // end else
		} // end if()
	} // end if()
} // CheckStandingVR()
*/