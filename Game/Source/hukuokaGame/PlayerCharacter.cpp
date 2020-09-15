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
#include "AutomaticDoorLever.h"
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
	, m_pCheckingActor(NULL)
	, m_pPrevCheckActor(NULL)
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

	// スマホを構える・構えを解除(作成者：尾崎)
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);
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

	UE_LOG(LogTemp, Log, TEXT("newRotationPlayer(%.2f, %.2f, %.2f)"),newRotationPlayer.Roll, newRotationPlayer.Pitch, newRotationPlayer.Yaw);
	
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

	// 1フレーム前のActorの情報を移す
	m_pPrevCheckActor = m_pCheckingActor;

	if (GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_GameTraceChannel3))
	{
		m_pCheckingActor = outHit.GetActor();
		// 1フレーム前のActorと違うなら更新
		if (m_pCheckingActor != m_pPrevCheckActor)
		{
			// チェック中のアクターを更新する
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
		// 前フレームでは検知していた場合そのActorの接触フラグを下げる
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
	UE_LOG(LogTemp, Warning, TEXT("R is Pressed"));
	if (m_pCheckingActor != NULL)
	{
		// 拾えるActor-->拾う(ここでは対象のActorを消す)
		if (m_pCheckingActor->ActorHasTag("CanPickUpActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Picked up %s."), *m_pCheckingActor->GetName()));

			// 何かしての拾ったActorの情報を保存

			// 拾ったActorを削除
			m_pCheckingActor->Destroy();
			m_pCheckingActor = NULL;
		}
		// 調べられるActor-->調べる
		else if (m_pCheckingActor->ActorHasTag("CanCheckActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Check %s."), *m_pCheckingActor->GetName()));
			// 説明文表示
		}
		// 作動させることができるActor-->作動
		else if (m_pCheckingActor->ActorHasTag("CanActuateActor"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Actuate %s."), *m_pCheckingActor->GetName()));

			// レバーを反転(現状作動させるActorはドアのレバーのみ)
			AAutomaticDoorLever* pLever = Cast<AAutomaticDoorLever>(m_pCheckingActor);
			pLever->m_isLeverOn = !pLever->m_isLeverOn;
		}
	}
}

// プレイヤーアクション：スマホのシャッターフラグを変更(作成者：尾崎)
bool APlayerCharacter::GetShatterFlag()
{
	return shatterFlag;
}

// プレイヤーアクション：スマホのライトのフラグを変更(作成者：尾崎)
bool APlayerCharacter::GetLightFlag()
{
	return lightFlag;
}

// プレイヤーアクション：スマホを構える状態のフラグを変更(作成者：尾崎)
bool APlayerCharacter::GetisHaveSmartphoneFlag()
{
	return isHaveSmartphoneFlag;
}