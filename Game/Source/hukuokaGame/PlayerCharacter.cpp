//-------------------------------------------------------------------
// �t�@�C��		�FPlayerCharacter.cpp
// �T�v			�F�v���C���[�L�����𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/07
//-------------------------------------------------------------------

#include "PlayerCharacter.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

// �R���X�g���N�^
APlayerCharacter::APlayerCharacter()
	: m_playerThresholdToRun(1.0f)
	, m_playerRunSpeed(10.0f)
	, m_playerWalkSpeed(5.0f)
	, m_cameraPitchLimitMin(-90.0f)
	, m_cameraPitchLimitMax(90.0f)
	, m_playerMoveSpeed(0.0f)
	, m_playerMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
{
 	// �e�B�b�N���Ăяo�����̃t���O
	PrimaryActorTick.bCanEverTick = true;

	// �J�����𐶐�
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));
	// �J�v�Z���R���C�_�[�ɃJ�������A�^�b�`
	m_pCamera->SetupAttachment(GetCapsuleComponent());
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

}

// ���t���[���Ă΂��
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �J�����̍X�V
	UpdateCamera(DeltaTime);

	// �v���C���[�L�����N�^�[�̍X�V
	UpdatePlayerMove(DeltaTime);

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
}
// �J�����̍X�V
void APlayerCharacter::UpdateCamera(const float _deltaTime)
{
	// ���݂̃v���C���[�̉�]�����擾
//	FRotator newRotationPlayer = GetActorRotation();

	// ���݂̃J�����̉�]�����擾
	FRotator newRotationCamera = m_pCamera->GetRelativeRotation();

	// Yaw(�v���C���[����]������)
//	newRotationPlayer.Yaw += m_cameraRotateInput.X;

	// Yaw
	newRotationCamera.Yaw += m_cameraRotateInput.X;
	
	// Pitch(�J��������]������)
	newRotationCamera.Pitch = FMath::Clamp((newRotationCamera.Pitch - m_cameraRotateInput.Y), m_cameraPitchLimitMin, m_cameraPitchLimitMax);

	// �v���C���[�A�J�������ꂼ��ɉ�]����ݒ�
	m_pCamera->SetRelativeRotation(newRotationCamera);
}

// �v���C���[�̈ړ�����
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// �x�N�g���̒������擾
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// �ړ��ʂ�����
	// ����
	if (vectorLength >= m_playerThresholdToRun)
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
		m_playerMoveSpeed = m_playerRunSpeed;
	}
	// ����
	else if (vectorLength > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
		m_playerMoveSpeed = m_playerWalkSpeed;
	}
	// �~�܂�
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
		m_playerMoveSpeed = 0.0f;
	}

	// �x�N�g���̐��K��
	NormalizedVector2D(vectorLength, &m_playerMoveInput);
	// �v���C���[�̌����Ă���������擾���e�ړ��ʂ�n��(if��0�����邱�Ƃ�������邽��)
	if (m_playerMoveInput.X != 0)
	{
		FVector forwardVec = GetActorForwardVector();
		AddMovementInput(forwardVec, m_playerMoveSpeed * m_playerMoveInput.X);
	}
	if (m_playerMoveInput.Y != 0)
	{
		FVector rightVec = GetActorRightVector();
		AddMovementInput(rightVec, m_playerMoveSpeed * m_playerMoveInput.Y);
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	// ���͒l���O
	UE_LOG(LogTemp, Log, TEXT("LeftStick(X, Y) = (%.2f, %.2f) RightStick(X, Y) = (%.2f, %.2f)"),
		m_playerMoveInput.X, m_playerMoveInput.Y, m_cameraRotateInput.X, m_cameraRotateInput.Y);

	// �v���C���[�̌��݂�ForwardVec
	UE_LOG(LogTemp, Log, TEXT("ForwardVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
		GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
	// �v���C���[�̌��݂�RightVec
	UE_LOG(LogTemp, Log, TEXT("RightVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
		GetActorRightVector().X, GetActorRightVector().Y, GetActorRightVector().Z);

	// �e�x�N�g���̈ړ���
	UE_LOG(LogTemp, Log, TEXT("forward : %.2f  right : %.2f"),
		m_playerMoveSpeed * m_playerMoveInput.X, m_playerMoveSpeed * m_playerMoveInput.Y);

	// �x�N�g���̒������O
	UE_LOG(LogTemp, Log, TEXT("VectorLength is %.2f"),
		ReturnVector2DLength(&m_playerMoveInput));
	////-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
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
	m_cameraRotateInput.Y = _axisValue;
}
// �J������]�FYaw(Z��)
void APlayerCharacter::CameraRotateYaw(float _axisValue)
{
	m_cameraRotateInput.X = _axisValue;
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