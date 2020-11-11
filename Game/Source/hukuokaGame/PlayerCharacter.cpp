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
//				�F2020/10/14		���g�̐���̒ǉ�
//				�F2020/11/04		�S�����A�v���̃A�N�V�����}�b�s���O��ǉ�
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
//-------------------------------------------------------------------


#include "PlayerCharacter.h"
#include "ItemBase.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VirtualReality/ThrillerVR_MotionController.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
// �ȏ��6�_��VR�p�C���N���[�h by_Rin
#include "LevelSwitchHelper.h"	// �}�b�v�J�ڗp�N���X
#include "GameFramework/Actor.h" // makeNoise�p


#define LASERLENGTH 300.0f		// VR�p LASER�̒���(��̒������\���邱��) by_Rin

// �R���X�g���N�^
APlayerCharacter::APlayerCharacter()
	: player_state(0)
	, m_playerThresholdToRun(1.0f)
	, m_playerRunSpeed(10.0f)
	, m_playerWalkSpeed(5.0f)
	, player_footstep_span_(200.0f)
	, m_cameraPitchLimitMin(-89.0f)
	, m_cameraPitchLimitMax(89.0f)
	, m_pCamera(NULL)
	, sound_player_footstep_(NULL)
	, m_eyeLevelWhenStanding(170.0f)
	, camera_shaking_value(10.0f)
	, m_reverseInputPitch(false)
	, m_reverseInputYaw(false)
	, m_cameraRotateSpeed(100.0f)
	, m_CheckToActorRayRange(1300.0f)
	, m_isStanding(true)
	, count_for_footstep_(0.0f)
	, eyelevel_for_camera_shaking(0.0f)
	, can_make_footstep(true)
	, can_player_control(true)
	, m_playerMoveSpeed(0.0f)
	, m_playerMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_pCheckingItem(NULL)
	, m_pPrevCheckItem(NULL)
	, vr_HitResult(NULL)
	, vr_InCameraMode(false)
	, vr_SmartPhone_Mission_Num(1)
	, isFound(false)
	, isHeartBeatOn(false)
	, m_MaxWalkSpeed_Walk(250.0f)
	, m_MaxWalkSpeed_Run(500.0f)
	, m_MaxWalkSpeed_Crouch(150.0f)
	, m_VRPlayersHeight(175.0f)
	, m_HeightisChecked(false)
	, holdingSmartphoneState(0)
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
	m_pSpringArm->bEnableCameraLag = true;

	// �J�����Ǐ]���O�̑��x��ݒ�
	m_pSpringArm->CameraLagSpeed = 20.f;

	// �J�������_�̐���
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// �J�����𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// �J�������_�ɃX�v�����O�A�[�����A�^�b�`
	m_pCameraBase->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);	

	// �J�������_�ɃJ�������A�^�b�`
	m_pCamera->SetupAttachment(m_pCameraBase);
}

// �f�X�g���N�^
APlayerCharacter::~APlayerCharacter()
{
}

// ���s���Ɉ�x�Ă΂��
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ===========  �v���C���[�ړ��A���Ⴊ�ޗp�̃v���p�e�B�ݒ�  by_Rin ===========
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;					// ���Ⴊ�ނ��\���܂�
	GetCharacterMovement()->CrouchedHalfHeight = 40.f;							// ���Ⴊ�ގ��̍���
	GetCharacterMovement()->bShrinkProxyCapsule = true;							// ���Ⴊ�ގ���collision�̕ύX���\���܂�(�K�v�Ȃ�����)
	GetCharacterMovement()->MaxWalkSpeedCrouched = m_MaxWalkSpeed_Crouch;		// ���Ⴊ�ގ��̈ړ����x
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

	// ===========  VR Motion Controller's Spawn and Attach  by_Rin ===========
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
					vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ��

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
			vr_Phone->SetActorRelativeLocation(FVector(200, 0, 10));

			// VR�X�}�z�̃T�C�Y
			vr_Phone->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));

			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
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

	// �A�C�e���̃`�F�b�N
	CheckItem();

	// ===========  VR Motion Controller's Laser Update by_Rin ===========
	// ����VR���[�h?
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// VR �R���g���[���[ �|�C���^�[�̍X�V
		UpdateVRLaser();
	} // end if()

	//// �f�o�b�O�m�F�p(�����������ǂ���)
	//UE_LOG(LogTemp, Warning, TEXT("isFound = %d"), isFound);

	// �G�ɕ߂܂�����
	if (isFound)
	{
		// �G�̃A�j���[�V�����I���܂�Respawn�֐��͌Ă΂Ȃ�

		// ���X�|�[���֐����Ăяo��
		Respawn();
	}
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

	// �X�}�z���\����E�\��������(�쐬�ҁF����)�@���͎g���Ă��� by_Rin
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);

	// �S�����A�v���̐؂�ւ�
	InputComponent->BindAction("HeartBeatStatusSwitch", IE_Pressed, this, &APlayerCharacter::HeartBeatStatusSwitch);
}

// �J����(Pitch)�̍X�V
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
	// ����s�Ȃ�return
	if (!can_player_control)	return;

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
	if (!can_player_control)	return;

	// ���݂̃v���C���[�̉�]�����擾
	FRotator newRotationPlayer = GetActorRotation();
	
	// VR's ��] by_Rin
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
	if (!can_player_control)	return;

	// �x�N�g���̒������擾
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// �ړ��ʂ�����		�C��by_Rin
	if (m_isStanding == true && (GetCharacterMovement()->IsCrouching() == false))
	{
		/*
		// unreal engine's bug, dont use it. Using BluePrint Now.
			GetCharacterMovement()->UnCrouch(true);
		*/

		// ����
		if (vectorLength >= 0.5f)
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
			m_playerMoveSpeed = m_playerRunSpeed;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Run;
		}
		// ����
		else if (vectorLength > 0.0f && vectorLength < 0.5f)
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
			m_playerMoveSpeed = m_playerWalkSpeed;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Walk;
		}
		// �~�܂�
		else
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
			m_playerMoveSpeed = 0.0f;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Walk;
		}
	} // end if()
	// ���Ⴊ��ł����ꍇ�ړ����x��1/2��
	else
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Squat"));
		GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Crouch;
		m_playerMoveSpeed = m_playerWalkSpeed / 2.0f;

		/*
		// unreal engine's bug, dont use it. Using BluePrint Now.
		if (GetCharacterMovement()->IsCrouching() == false)
		{
			GetCharacterMovement()->Crouch(true);
		} // end if
		*/
	} // end else 

	// �x�N�g���̐��K��
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// �v���C���[�̈ړ����x�m�F�p
	GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Purple, FString::SanitizeFloat(GetCharacterMovement()->Velocity.Size()));

	// �n�ʂƂ̋����𑪂�v���C���[�̍�����ݒ�
	SetEyeLevel(_deltaTime, (m_playerMoveSpeed * _deltaTime));

	
	// VR���̈ړ� by_Rin
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
	// C++�̂��Ⴊ�ނ��g���Ȃ�����, �R�����g�A�E�g���܂����iBluePrint�ɐݒ肵�Ă���j  by_Rin
	// m_isStanding = true;
}

// �v���C���[�A�N�V�����F���Ⴊ��
void APlayerCharacter::PlayerSquat() {
	// C++�̂��Ⴊ�ނ��g���Ȃ�����, �R�����g�A�E�g���܂����iBluePrint�ɐݒ肵�Ă���j	by_Rin
	// m_isStanding = false;
}

// �n�ʂƂ̋����𑪂�v���C���[�̍�����ݒ�
void APlayerCharacter::SetEyeLevel(const float _deltaTime, const float _player_move_speed)
{
	// �v���C���[�̕��s�ɂ�鎋���̏c�h��
	eyelevel_for_camera_shaking = ReturnCameraVerticalShaking(_deltaTime, _player_move_speed);

	// �����Ă���΂��̂܂܂��Ⴊ��ł���΃A�C���x����1/4�ɂ��č��W�Z�b�g
	// ���Ⴊ�ނƍ��킹�Ă̏�����������	by_Rin
	if (m_isStanding && (GetCharacterMovement()->IsCrouching() == false))
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Stand eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, ( m_eyeLevelWhenStanding + eyelevel_for_camera_shaking)));

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

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, ((m_eyeLevelWhenStanding / 4) + eyelevel_for_camera_shaking)));
		

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
	if (!can_player_control)
	{
		// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
		OnItemCheckEndEventDispatcher.Broadcast();

		return;
	}

	// �g���[�X�ɕK�v�ȕϐ���錾
	FHitResult outHit;

	// ���݈ʒu���擾
	FVector start = m_pCamera->GetComponentLocation();
	FVector end = start + (m_pCamera->GetForwardVector() * m_CheckToActorRayRange);

	// 1�t���[���O�̃A�C�e���̏����ڂ�
	m_pPrevCheckItem = m_pCheckingItem;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel3))
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

// PC�ŁA�X�}�z����O�Ɏ����Ă��邩  by_Rin
void APlayerCharacter::ChangeHaveSmartphoneFlag()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
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
} // ChangeHaveSmartphoneFlag()

AItemBase* APlayerCharacter::ReturnCheckingItem() const
{
	return m_pCheckingItem;
}

FString APlayerCharacter::ReturnCheckingItemCommandName() const
{
	return m_pCheckingItem->m_commandName;
}

FVector APlayerCharacter::ReturnCameraForwardVector()
{ 
	FVector forward_vector = FVector::ZeroVector;

	if (m_pCamera != NULL)	forward_vector = m_pCamera->GetForwardVector();

	return forward_vector;
}

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


// ===========  VR HMD�̃��Z�b�g  by_Rin ===========
void APlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
} // void APlayerCharacter::OnResetVR()

// =====  VR Motion �R���g���[���[ �|�C���^�[�̊֐�  by_Rin =====
void APlayerCharacter::UpdateVRLaser()
{
	FVector StartPoint = FVector::ZeroVector;			// Laser�̏����ʒu
	FVector FowardActor = FVector::ZeroVector;			// ���[�V�����R���g���[���[�̑O����

	// VR's laser start point from right controller
	if (RightController)
	{
		StartPoint = RightController->ActorToWorld().GetLocation();
		FowardActor = RightController->GetRootComponent()->GetChildComponent(0)->GetForwardVector();
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

	// Laser���ŏ��ɓ��������̂��̂��@vr_HitResult�@�ɔ��f����
	if (GetWorld()->LineTraceSingleByChannel(vr_HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
	{
		if (vr_InCameraMode == false)
		{
			LineBatcher->DrawLine(StartPoint, EndPoint, FLinearColor::Red, 10, 0.f, 1.f);
		} // end if()

		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("RayHit: %s"), *HitResult.Actor->GetName()));
	} // end if()
	else
	{
		if (vr_InCameraMode == false)
		{
			LineBatcher->DrawLine(StartPoint, EndPoint, FLinearColor::Green, 10, 0.f, 1.f);
		} // end if()
	} // end else

} // APlayerCharacter::UpdateVRLaser()

// ���X�|�[���֐�(�쐬�ҁF����)
void APlayerCharacter::Respawn()
{
	//--- �Q�[���I�[�o�[�ɑJ�ڂ̒ǉ� by ��K
	if (GetWorld()->GetName() == "ProtoType")
	{
		LevelSwitchHelper::OpenGameOverLevelMap(this);
	}

	// �t���O�̏�����
	isFound = false;

	//// �f�o�b�O�m�F�p(���񂾂Ƃ��̃��O)
	//UE_LOG(LogTemp, Warning, TEXT("Player Dead... Respawn."));

	// �ʒu�̏�����
	this->SetActorLocation(FVector(-4850.f, -3300.f, 300.f));
}

// C++�̂��Ⴊ�ނ��g���Ȃ�����
// CheckStandingVR��������
// by_Rin
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