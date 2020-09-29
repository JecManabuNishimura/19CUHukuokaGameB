//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：プレイヤーキャラを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：VRカメラの作成
// 作成者		：19CU0217 朱適
// 作成日		：2020/08/18
//-------------------------------------------------------------------

#include "PlayerCharacter.h"
#include "ItemBase.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"


// コンストラクタ
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
{
 	// ティックを呼び出すかのフラグ
	PrimaryActorTick.bCanEverTick = true;

	// デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// カメラ原点の生成
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// カメラを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// カプセルコライダーにカメラ原点をアタッチ
	m_pCameraBase->SetupAttachment(RootComponent);

	// カメラ原点にカメラをアタッチ
	m_pCamera->SetupAttachment(m_pCameraBase);
}

// デストラクタ
APlayerCharacter::~APlayerCharacter()
{
}

// 実行時に一度呼ばれる
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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

// 毎フレーム呼ばれる
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 地面のとの距離を測りプレイヤーの高さを設定
	SetEyeLevel(DeltaTime);

	// カメラ(Pitch)の更新
	UpdateCameraPitch(DeltaTime);

	// カメラ(Yaw)の更新
	UpdateCameraYaw(DeltaTime);

	// プレイヤーキャラクターの更新
	UpdatePlayerMove(DeltaTime);

	// アイテムのチェック
	CheckItem();
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

	// 立ち上がる、しゃがむ
	InputComponent->BindAction("PlayerSquat", IE_Pressed, this, &APlayerCharacter::PlayerSquat);
	InputComponent->BindAction("PlayerSquat", IE_Released, this, &APlayerCharacter::PlayerStand);

	// プレイヤーアクション：拾う、調べる、作動させる
	InputComponent->BindAction("PickUpandCheck", IE_Released, this, &APlayerCharacter::CheckToActor);
}

// 地面との距離を測りプレイヤーの高さを設定
void APlayerCharacter::SetEyeLevel(const float _deltaTime)
{
	// トレースに必要な変数を宣言
	FHitResult outHit;

	// 現在位置を取得
	FVector startLocation = GetActorLocation();

	if (GetWorld()->LineTraceSingleByChannel(outHit, startLocation, (startLocation + FVector(0.0f, 0.0f, -300.0f)), ECC_GameTraceChannel2))
	{
		// トレースがヒットしたZ座標を取得
		float hitLocationZ = outHit.Location.Z;

		// 座標設定の際、現在のZ座標の情報はいらないので0に変更
		startLocation.Z = 0.0f;

		// 立っていれば設定したアイレベルのまま
		if (m_isStanding)
		{
			SetActorLocation(startLocation + FVector(0.0f, 0.0f, (hitLocationZ + m_eyeLevelWhenStanding)));
			GEngine->AddOnScreenDebugMessage(13, _deltaTime, FColor::Green, TEXT("Player is Standing"));
		}
		// しゃがんでいれば設定したアイレベルの1/2の高さにする
		else
		{
			SetActorLocation(startLocation + FVector(0, 0, (hitLocationZ + (m_eyeLevelWhenStanding / 2.0f))));
			GEngine->AddOnScreenDebugMessage(13, _deltaTime, FColor::Green, TEXT("Player is Squatting"));
		}

		//--------------------------------------------------------------------------------------------------
		// ログ
		// ヒットしたオブジェクトの名前を表示TEX
		//UE_LOG(LogClass, Log, TEXT("The Component Being Hit is: %s"), *outHit.GetActor()->GetName());
		//UE_LOG(LogClass, Log, TEXT("Hit Z Location : %.2f"), outHit.Location.Z);
		//--------------------------------------------------------------------------------------------------
	}
}

// カメラ(Pitch)の更新
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
	if (m_pCamera != NULL)
	{
		// 現在のカメラの回転情報を取得
		FRotator newRotationCamera = m_pCamera->GetRelativeRotation();

		// Pitch(カメラを回転させる)
		newRotationCamera.Pitch = FMath::Clamp((newRotationCamera.Pitch - (m_cameraRotateInput.Y * m_cameraRotateSpeed * _deltaTime)), m_cameraPitchLimitMin, m_cameraPitchLimitMax);

		// カメラに回転情報を設定
		m_pCamera->SetRelativeRotation(newRotationCamera);
	}
}

// カメラ(Yaw)の更新
void APlayerCharacter::UpdateCameraYaw(const float _deltaTime)
{
	// 現在のプレイヤーの回転情報を取得
	FRotator newRotationPlayer = GetActorRotation();
	
	// Yaw(プレイヤーを回転させる)
	newRotationPlayer.Yaw += m_cameraRotateInput.X * m_cameraRotateSpeed * _deltaTime;

	// プレイヤーに回転情報を設定
	SetActorRotation(newRotationPlayer);
}

// プレイヤーの移動処理
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// ベクトルの長さを取得
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	FVector newLocation = GetActorLocation();

	// 移動量を決定
	// 走る
	if (vectorLength >= m_playerThresholdToRun)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
		m_playerMoveSpeed = m_playerRunSpeed;
	}
	// 歩く
	else if (vectorLength > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
		m_playerMoveSpeed = m_playerWalkSpeed;
	}
	// 止まる
	else
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
		m_playerMoveSpeed = 0.0f;
	}

	// ベクトルの正規化
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// しゃがんでいた場合移動速度を1/2に
	if (!m_isStanding)
	{
		m_playerMoveSpeed /= 2.0f;
	}

	// 移動量加算
	newLocation += GetActorForwardVector() * (m_playerMoveSpeed * m_playerMoveInput.X * _deltaTime);
	newLocation += GetActorRightVector() * (m_playerMoveSpeed * m_playerMoveInput.Y * _deltaTime);

	SetActorLocation(newLocation);

	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
	// 入力値ログ
	//UE_LOG(LogTemp, Log, TEXT("LeftStick(X, Y) = (%.2f, %.2f) RightStick(X, Y) = (%.2f, %.2f)"),
	//	m_playerMoveInput.X, m_playerMoveInput.Y, m_cameraRotateInput.X, m_cameraRotateInput.Y);

	//// プレイヤーの現在のForwardVec
	//UE_LOG(LogTemp, Log, TEXT("ForwardVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
	//	GetActorForwardVector().X, GetActorForwardVector().Y, GetActorForwardVector().Z);
	////// プレイヤーの現在のRightVec
	//UE_LOG(LogTemp, Log, TEXT("RightVec(X, Y, Z) : (%.2f, %.2f, %.2f)"),
	//	GetActorRightVector().X, GetActorRightVector().Y, GetActorRightVector().Z);

	//// 各ベクトルの移動量
	//UE_LOG(LogTemp, Log, TEXT("forward : %.2f  right : %.2f"),
	//	m_playerMoveSpeed * m_playerMoveInput.X, m_playerMoveSpeed * m_playerMoveInput.Y);

	//// ベクトルの長さログ
	//UE_LOG(LogTemp, Log, TEXT("VectorLength is %.2f"),
	//	ReturnVector2DLength(&m_playerMoveInput));
	////-----------------------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------------------
}

void APlayerCharacter::CheckItem()
{
	// トレースに必要な変数を宣言
	FHitResult outHit;

	// 現在位置を取得
	FVector start = m_pCamera->GetComponentLocation();
	FVector end = start + (m_pCamera->GetForwardVector() * m_CheckToActorRayRange);

	// 1フレーム前のアイテムの情報を移す
	m_pPrevCheckItem = m_pCheckingItem;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel3))
	{
		// アイテム基本クラスにキャスト
		m_pCheckingItem = Cast<AItemBase>(outHit.GetActor());

		// キャストが成功していれば処理を続行
		if (m_pCheckingItem != NULL)
		{
			// 1フレーム前のアイテムと違うなら更新
			if (m_pCheckingItem != m_pPrevCheckItem)
			{
				// 前フレームで有効なオブジェクトをチェックしていたら
				if (m_pPrevCheckItem != NULL)
				{
					// 前フレームでチェックしていたオブジェクトの被チェックを無効に
					m_pPrevCheckItem->m_isChecked = false;

					// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
					OnItemCheckEndEventDispatcher.Broadcast();
				}
				// 新しくチェックしたオブジェクトの被チェックを有効に
				m_pCheckingItem->m_isChecked = true;

				// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートに追加)
				OnItemCheckBeginEventDispatcher.Broadcast();
			}
		}
	}
	else
	{
		// 前フレームでは検知していた場合そのアイテムの接触フラグを下げる
		if (m_pPrevCheckItem != NULL)
		{
			m_pPrevCheckItem->m_isChecked = false;

			// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
			OnItemCheckEndEventDispatcher.Broadcast();
		}
		m_pCheckingItem = NULL;
	}
}

// ベクトルの長さを返す
float APlayerCharacter::ReturnVector2DLength(const FVector2D* _pFvector2d)
{
	return FMath::Sqrt((_pFvector2d->X * _pFvector2d->X) + (_pFvector2d->Y * _pFvector2d->Y));
}

// ベクトルを正規化する
void APlayerCharacter::NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d)
{
	float multipleNum = 1.0f / _vectorLength;

	_pFvector2d->X *= multipleNum;
	_pFvector2d->Y *= multipleNum;
}

// 入力バインド
// カメラ回転：Pitch(Y軸)
void APlayerCharacter::CameraRotatePitch(float _axisValue)
{
	// Pitchの操作反転フラグが立っていたら反転
	if (m_reverseInputPitch) m_cameraRotateInput.Y = _axisValue * -1.0f;
	// 降りていたらそのままの入力値
	else                     m_cameraRotateInput.Y = _axisValue;
}
// カメラ回転：Yaw(Z軸)
void APlayerCharacter::CameraRotateYaw(float _axisValue)
{
	// Pitchの操作反転フラグが立っていたら反転
	if (m_reverseInputYaw) m_cameraRotateInput.X = _axisValue * -1.0f;
	// 降りていたらそのままの入力値
	else                   m_cameraRotateInput.X = _axisValue;
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
// プレイヤーアクション：拾う、調べる、作動させる
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