//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�F�v���C���[�L�����𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�FVR�J�����̍쐬
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/08/18
//-------------------------------------------------------------------

#include "PlayerCharacter.h"
#include "AutomaticDoorLever.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"


// �R���X�g���N�^
APlayerCharacter::APlayerCharacter()
	: m_playerThresholdToRun(1.0f)
	, m_playerRunSpeed(10.0f)
	, m_playerWalkSpeed(5.0f)
	, m_cameraPitchLimitMin(-89.0f)
	, m_cameraPitchLimitMax(89.0f)
	, m_pCamera(NULL)
	, m_eyeLevelWhenStanding(170.0f)
	, m_reverseInputPitch(false)
	, m_reverseInputYaw(false)
	, m_cameraRotateSpeed(100.0f)
	, m_CheckToActorRayRange(1300.0f)
	, m_isStanding(true)
	, m_playerMoveSpeed(0.0f)
	, m_playerMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_pCheckingActor(NULL)
	, m_pPrevCheckActor(NULL)
{
 	// �e�B�b�N���Ăяo�����̃t���O
	PrimaryActorTick.bCanEverTick = true;

	// �f�t�H���g�v���C���[�Ƃ��Đݒ�
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// �J�������_�̐���
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// �J�����𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// �J�v�Z���R���C�_�[�ɃJ�������_���A�^�b�`
	m_pCameraBase->SetupAttachment(RootComponent);

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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("We are using PlayerCharacter."));
	}

	// Epic Comment :D // Setup Player Height for various Platforms (PS4, Vive, Oculus)
	FName DeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	if (DeviceName == "SteamVR" || DeviceName == "OculusHMD")
	{
		// Epic Comment :D // Windows (Oculus / Vive)
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Can't find VR Divice"));
	}
}

// ���t���[���Ă΂��
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �n�ʂ̂Ƃ̋����𑪂�v���C���[�̍�����ݒ�
	SetEyeLevel(DeltaTime);

	// �J����(Pitch)�̍X�V
	UpdateCameraPitch(DeltaTime);

	// �J����(Yaw)�̍X�V
	UpdateCameraYaw(DeltaTime);

	// �v���C���[�L�����N�^�[�̍X�V
	UpdatePlayerMove(DeltaTime);

	// �A�C�e���̃`�F�b�N
	CheckItem();
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

	// �X�}�z���\����E�\��������(�쐬�ҁF����)
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);
}

// �n�ʂƂ̋����𑪂�v���C���[�̍�����ݒ�
void APlayerCharacter::SetEyeLevel(const float _deltaTime)
{
	// �g���[�X�ɕK�v�ȕϐ���錾
	FHitResult outHit;

	// ���݈ʒu���擾
	FVector startLocation = GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(outHit, startLocation, (startLocation + FVector(0.0f, 0.0f, -300.0f)), ECC_GameTraceChannel2))
	{
		// �g���[�X���q�b�g����Z���W���擾
		float hitLocationZ = outHit.Location.Z;

		// ���W�ݒ�̍ہA���݂�Z���W�̏��͂���Ȃ��̂�0�ɕύX
		startLocation.Z = 0.0f;

		// �����Ă���ΐݒ肵���A�C���x���̂܂�
		if (m_isStanding)
		{
			SetActorLocation(startLocation + FVector(0.0f, 0.0f, (hitLocationZ + m_eyeLevelWhenStanding)));
			GEngine->AddOnScreenDebugMessage(13, _deltaTime, FColor::Green, TEXT("Player is Standing"));
		}
		// ���Ⴊ��ł���ΐݒ肵���A�C���x����1/2�̍����ɂ���
		else
		{
			SetActorLocation(startLocation + FVector(0, 0, (hitLocationZ + (m_eyeLevelWhenStanding / 2.0f))));
			GEngine->AddOnScreenDebugMessage(13, _deltaTime, FColor::Green, TEXT("Player is Squatting"));
		}

		//--------------------------------------------------------------------------------------------------
		// ���O
		// �q�b�g�����I�u�W�F�N�g�̖��O��\��TEX
		//UE_LOG(LogClass, Log, TEXT("The Component Being Hit is: %s"), *outHit.GetActor()->GetName());
		//UE_LOG(LogClass, Log, TEXT("Hit Z Location : %.2f"), outHit.Location.Z);
		//--------------------------------------------------------------------------------------------------
	}
}

// �J����(Pitch)�̍X�V
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
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
	// ���݂̃v���C���[�̉�]�����擾
	FRotator newRotationPlayer = GetActorRotation();

	UE_LOG(LogTemp, Log, TEXT("newRotationPlayer(%.2f, %.2f, %.2f)"),newRotationPlayer.Roll, newRotationPlayer.Pitch, newRotationPlayer.Yaw);
	
	// Yaw(�v���C���[����]������)
	newRotationPlayer.Yaw += m_cameraRotateInput.X * m_cameraRotateSpeed * _deltaTime;

	// �v���C���[�ɉ�]����ݒ�
	SetActorRotation(newRotationPlayer);
}

// �v���C���[�̈ړ�����
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// �x�N�g���̒������擾
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	FVector newLocation = GetActorLocation();

	// �ړ��ʂ�����
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

	// �x�N�g���̐��K��
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// ���Ⴊ��ł����ꍇ�ړ����x��1/2��
	if (!m_isStanding)
	{
		m_playerMoveSpeed /= 2.0f;
	}

	// �ړ��ʉ��Z
	newLocation += GetActorForwardVector() * (m_playerMoveSpeed * m_playerMoveInput.X * _deltaTime);
	newLocation += GetActorRightVector() * (m_playerMoveSpeed * m_playerMoveInput.Y * _deltaTime);

	SetActorLocation(newLocation);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	// ���͒l���O
	//UE_LOG(LogTemp, Log, TEXT("LeftStick(X, Y) = (%.2f, %.2f) RightStick(X, Y) = (%.2f, %.2f)"),
	//	m_playerMoveInput.X, m_playerMoveInput.Y, m_cameraRotateInput.X, m_cameraRotateInput.Y);

	//// �v���C���[�̌��݂�ForwardVec
	//UE_LOG(LogTemp, Log, TEXT("ForwardVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
	//	GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
	////// �v���C���[�̌��݂�RightVec
	//UE_LOG(LogTemp, Log, TEXT("RightVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
	//	GetActorRightVector().X, GetActorRightVector().Y, GetActorRightVector().Z);

	//// �e�x�N�g���̈ړ���
	//UE_LOG(LogTemp, Log, TEXT("forward : %.2f  right : %.2f"),
	//	m_playerMoveSpeed * m_playerMoveInput.X, m_playerMoveSpeed * m_playerMoveInput.Y);

	//// �x�N�g���̒������O
	//UE_LOG(LogTemp, Log, TEXT("VectorLength is %.2f"),
	//	ReturnVector2DLength(&m_playerMoveInput));
	////-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void APlayerCharacter::CheckItem()
{
	// �g���[�X�ɕK�v�ȕϐ���錾
	FHitResult outHit;

	// ���݈ʒu���擾
	FVector start = m_pCamera->GetComponentLocation();
	FVector end = start + (m_pCamera->GetForwardVector() * m_CheckToActorRayRange);

	// 1�t���[���O��Actor�̏����ڂ�
	m_pPrevCheckActor = m_pCheckingActor;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel3))
	{
		m_pCheckingActor = outHit.GetActor();
		// 1�t���[���O��Actor�ƈႤ�Ȃ�X�V
		if (m_pCheckingActor != m_pPrevCheckActor)
		{
			// �`�F�b�N���̃A�N�^�[���X�V����
			AItemBase* itemBase = Cast<AItemBase>(m_pCheckingActor);
			if (itemBase != NULL)
			{
				itemBase->m_isChecked = true;
				if (m_pPrevCheckActor != NULL)
				{
					if (itemBase != NULL)
					{
						itemBase = Cast<AItemBase>(m_pPrevCheckActor);
						itemBase->m_isChecked = false;
					}
				}
			}
		}
		if (m_pCheckingActor != NULL)	GEngine->AddOnScreenDebugMessage(14, 0.05f, FColor::Blue, FString::Printf(TEXT("Checking %s."), *m_pCheckingActor->GetName()));
	}
	else
	{
		// �O�t���[���ł͌��m���Ă����ꍇ����Actor�̐ڐG�t���O��������
		if (m_pPrevCheckActor != NULL)
		{
			AItemBase* itemBase = Cast<AItemBase>(m_pCheckingActor);
			if (itemBase != NULL)
			{
				itemBase->m_isChecked = false;
			}
		}
		m_pCheckingActor = NULL;
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
	m_playerMoveInput.X = _axisValue;
}
// �v���C���[�ړ��F�ړ�Y������(��)
void APlayerCharacter::PlayerMoveY(float _axisValue)
{
	m_playerMoveInput.Y = _axisValue;
}
// �v���C���[�A�N�V�����F�E���A���ׂ�A�쓮������
void APlayerCharacter::CheckToActor()
{
	UE_LOG(LogTemp, Warning, TEXT("R is Pressed"));
	if (m_pCheckingActor != NULL)
	{
		// �E����Actor-->�E��(�����ł͑Ώۂ�Actor������)
		if (m_pCheckingActor->ActorHasTag("CanPickUpActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Picked up %s."), *m_pCheckingActor->GetName()));

			// �������Ă̏E����Actor�̏���ۑ�

			// �E����Actor���폜
			m_pCheckingActor->Destroy();
			m_pCheckingActor = NULL;
		}
		// ���ׂ���Actor-->���ׂ�
		else if (m_pCheckingActor->ActorHasTag("CanCheckActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Check %s."), *m_pCheckingActor->GetName()));
			// �������\��
		}
		// �쓮�����邱�Ƃ��ł���Actor-->�쓮
		else if (m_pCheckingActor->ActorHasTag("CanActuateActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Actuate %s."), *m_pCheckingActor->GetName()));

			// ���o�[�𔽓](����쓮������Actor�̓h�A�̃��o�[�̂�)
			AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(m_pCheckingActor);
			pLever->m_isLeverOn = !pLever->m_isLeverOn;
		}
	}
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