//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：プレイヤーキャラを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/07
//-------------------------------------------------------------------

#include "Engine.h"				// GEngineを呼び出すためのヘッダ
#include "PlayerCharacter.h"

// コンストラクタ
APlayerCharacter::APlayerCharacter()
	:m_playerThresholdToRun(1.0f)
	,m_playerRunSpeed(10.0f)
	,m_playerWalkSpeed(5.0f)
	,m_playerMoveSpeed(0.0f)
	,m_playerMoveInput(FVector2D::ZeroVector)
	,m_cameraRotateInput(FVector2D::ZeroVector)
{
 	// ティックを呼び出すかのフラグ
	PrimaryActorTick.bCanEverTick = true;

}

// デストラクタ
APlayerCharacter::~APlayerCharacter()
{
}

// 実行時に一度呼ばれる
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("We are using PlayerCharacter."));
	}

}

// 毎フレーム呼ばれる
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// カメラの更新
	UpdateCamera(DeltaTime);

	// プレイヤーキャラクターの更新
	UpdatePlayerMove(DeltaTime);

}

// 各入力関係メソッドとのバインド処理
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// プレイヤーの移動
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::PlayerMoveX);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::PlayerMoveY);

	// カメラの回転
	InputComponent->BindAxis("CameraPitch", this, &APlayerCharacter::CameraRotatePitch);
	InputComponent->BindAxis("CameraYaw", this, &APlayerCharacter::CameraRotateYaw);
}
// カメラの更新
void APlayerCharacter::UpdateCamera(const float _deltaTime)
{

}

// プレイヤーの移動処理
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// ベクトルの長さを取得
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// 移動量を決定
	// 走る
	if (vectorLength >= m_playerThresholdToRun)
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
		m_playerMoveSpeed = m_playerRunSpeed;
	}
	// 歩く
	else if (vectorLength > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
		m_playerMoveSpeed = m_playerWalkSpeed;
	}
	// 止まる
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
		m_playerMoveSpeed = 0.0f;
	}

	// プレイヤーの向いている方向を取得し各移動量を渡す(ifは0を割ることを回避するため)
	if (m_playerMoveInput.X != 0)
	{
		FVector forwardVec = GetActorForwardVector();
		AddMovementInput(forwardVec, m_playerMoveSpeed * (FMath::Abs(m_playerMoveInput.X) / m_playerMoveInput.X));
	}
	if (m_playerMoveInput.Y != 0)
	{
		FVector rightVec = GetActorRightVector();
		AddMovementInput(rightVec, m_playerMoveSpeed * (FMath::Abs(m_playerMoveInput.Y) / m_playerMoveInput.Y));
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 入力値ログ
	UE_LOG(LogTemp, Log, TEXT("LeftStick(X, Y) = (%.2f, %.2f) RightStick(X, Y) = (%.2f, %.2f)"),
		m_playerMoveInput.X, m_playerMoveInput.Y, m_cameraRotateInput.X, m_cameraRotateInput.Y);
	// ベクトルの長さログ
	UE_LOG(LogTemp, Log, TEXT("Right Stick VectorLength : %.2f"), vectorLength);
	// 各ベクトルの移動量
	UE_LOG(LogTemp, Log, TEXT("forward : %.2f  right : %.2f"),
		m_playerMoveSpeed * (FMath::Abs(m_playerMoveInput.X) / m_playerMoveInput.X), m_playerMoveSpeed * (FMath::Abs(m_playerMoveInput.Y) / m_playerMoveInput.Y));
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------
}

// ベクトルの長さを返す
float APlayerCharacter::ReturnVector2DLength(const FVector2D* _pFvector2d)
{
	return FMath::Sqrt((_pFvector2d->X * _pFvector2d->X) + (_pFvector2d->Y * _pFvector2d->Y));
}

// 入力バインド
// カメラ回転：Pitch(Y軸)
void APlayerCharacter::CameraRotatePitch(float _axisValue)
{
	m_cameraRotateInput.X = _axisValue;
}
// カメラ回転：Yaw(Z軸)
void APlayerCharacter::CameraRotateYaw(float _axisValue)
{
	m_cameraRotateInput.Y = _axisValue;
}

// プレイヤー移動：移動X軸方向(縦)
void APlayerCharacter::PlayerMoveX(float _axisValue)
{
	m_playerMoveInput.X = _axisValue;
}
// プレイヤー移動：移動Y軸方向(横)
void APlayerCharacter::PlayerMoveY(float _axisValue)
{
	m_playerMoveInput.Y = _axisValue;
}