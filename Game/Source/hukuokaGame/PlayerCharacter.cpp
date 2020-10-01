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

//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�FVR���[�V�����̑Ή�
// �쐬��		�F19CU0236 �щ_��
// �쐬��		�F2020/08/28	VR���[�V�����R���g���[���[�̑Ή�
// �X�V��		�F2020/09/06	VR��ray�̍쐬
//				�F2020/09/19	VR�̃X�}�[�g�t�H���쐬
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

#define LASERLENGTH 300.0f		// VR�p LASER�̒���(��̒������\���邱��) by_Rin

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
	, m_pCheckingItem(NULL)
	, m_pPrevCheckItem(NULL)
	, vr_HitResult(NULL)
	, vr_InCameraMode(false)
	, isFound(false)
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

			bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// path�ɂ���N���X���擾
			if (bp_VRphone != nullptr)
			{
				vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VR�̃X�}�[�g�t�H����Actor�Ƃ��Đ�������

				vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
				vr_Phone->SetActorEnableCollision(false);

				if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
				{
					// VR�p�z�u
					vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||
					vr_Phone->SetActorRelativeLocation(FVector(370, 0, 10));

					// �X�}�z���̕����A��Ɋm�F���܂��� (��(���b�V���̏��������Ή���)Y X Z)
					// vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

					// vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ��display
																								//   ||

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

	// 1�t���[���O�̃A�C�e���̏����ڂ�
	m_pPrevCheckItem = m_pCheckingItem;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel3))
	{
		// �A�C�e����{�N���X�ɃL���X�g
		m_pCheckingItem = Cast<AItemBase>(outHit.GetActor());

		// �L���X�g���������Ă���Ώ����𑱍s
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

					// �C�x���g�f�B�X�p�b�`���[�Ăяo��(�A�C�e���R�}���hUI���r���[�|�[�g�������)
					OnItemCheckEndEventDispatcher.Broadcast();
				}
				// �V�����`�F�b�N�����I�u�W�F�N�g�̔�`�F�b�N��L����
				m_pCheckingItem->m_isChecked = true;

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
	if (m_pCheckingItem != NULL)
	{
		m_pCheckingItem->CheckedByPlayer();
	}
}

AItemBase* APlayerCharacter::ReturnCheckingItem() const
{
	return m_pCheckingItem;
}

FString APlayerCharacter::ReturnCheckingItemCommandName() const
{
	return m_pCheckingItem->m_commandName;
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
	// �t���O�̏�����
	isFound = false;

	//// �f�o�b�O�m�F�p(���񂾂Ƃ��̃��O)
	//UE_LOG(LogTemp, Warning, TEXT("Player Dead... Respawn."));

	// �ʒu�̏�����
	this->SetActorLocation(FVector(-4850.f, -3300.f, 300.f));
}
