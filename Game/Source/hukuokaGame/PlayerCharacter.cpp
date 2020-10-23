//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：プレイヤーキャラを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 作成者		：19CU0217 朱適
// 更新日		：2020/08/18		VRカメラの作成
// 更新日		：2020/10/09		ゲームオーバーに遷移の機能を追加
// 更新日		：2020/10/14		白枠の制御の追加
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：VRモーションの対応
// 作成者		：19CU0236 林雲暉
// 作成日		：2020/08/28	VRモーションコントローラーの対応
// 更新日		：2020/09/06	VRのrayの作成
//				：2020/09/19	VRのスマートフォン作成
//-------------------------------------------------------------------


#include "PlayerCharacter.h"
#include "ItemBase.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VirtualReality/ThrillerVR_MotionController.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
// 以上の6点はVR用インクルード by_Rin
#include "LevelSwitchHelper.h"	// マップ遷移用クラス


#define LASERLENGTH 300.0f		// VR用 LASERの長さ(手の長さを代表すること) by_Rin

// コンストラクタ
APlayerCharacter::APlayerCharacter()
	: m_playerThresholdToRun(1.0f)
	, m_playerRunSpeed(10.0f)
	, m_playerWalkSpeed(5.0f)
	, player_footstep_span_(200.0f)
	, m_cameraPitchLimitMin(-89.0f)
	, m_cameraPitchLimitMax(89.0f)
	, m_pCamera(NULL)
	, m_eyeLevelWhenStanding(170.0f)
	, camera_shaking_value(10.0f)
	, m_reverseInputPitch(false)
	, m_reverseInputYaw(false)
	, m_cameraRotateSpeed(100.0f)
	, m_CheckToActorRayRange(1300.0f)
	, m_isStanding(true)
	, sound_player_footstep_(NULL)
	, count_for_footstep_(0.0f)
	, eyelevel_for_camera_shaking(0.0f)
	, can_make_footstep(true)
	, m_playerMoveSpeed(0.0f)
	, m_playerMoveInput(FVector2D::ZeroVector)
	, m_cameraRotateInput(FVector2D::ZeroVector)
	, m_pCheckingItem(NULL)
	, m_pPrevCheckItem(NULL)
	, vr_HitResult(NULL)
	, vr_InCameraMode(false)
	, isFound(false)
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

	// 足音のSEを検索、保存
	ConstructorHelpers::FObjectFinder<USoundBase>find_sound(TEXT("/Game/SE/Player_Footstep_Cue"));
	if (find_sound.Succeeded()) { sound_player_footstep_ = find_sound.Object; }
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
	} // end if()
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Can't find VR Divice"));
	} // end else

	// ===========  VR Motion Controller's Spawn and Attach  by_Rin ===========
	// もしタイトル画面にモードを選択するなら、ifの条件を変えます
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

			bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// pathにあるクラスを取得
			if (bp_VRphone != nullptr)
			{
				vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VRのスマートフォンをActorとして生成する

				vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
				vr_Phone->SetActorEnableCollision(false);

				if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
				{
					// VR用配置
					vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||
					vr_Phone->SetActorRelativeLocation(FVector(370, 0, 10));

					// スマホ他の方向、先に確認しました (仮(メッシュの初期方向対応め)Y X Z)
					// vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

					// vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ↑display
																								//   ||

					// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
				} // end if
				else
				{
					// PC確認用配置
					vr_Phone->SetActorRelativeRotation(FRotator(-90.f, -180.f, 180.f));
					vr_Phone->SetActorRelativeLocation(FVector(400, 0, 70));
				} // end else

				// 確認用
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

// 毎フレーム呼ばれる
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// カメラ(Pitch)の更新
	UpdateCameraPitch(DeltaTime);

	// カメラ(Yaw)の更新
	UpdateCameraYaw(DeltaTime);

	// プレイヤーキャラクターの更新
	UpdatePlayerMove(DeltaTime);

	// アイテムのチェック
	CheckItem();

	// ===========  VR Motion Controller's Laser Update by_Rin ===========
	// 今はVRモード?
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// VR コントローラー ポインターの更新
		UpdateVRLaser();
	} // end if()

	//// デバッグ確認用(見つかったかどうか)
	//UE_LOG(LogTemp, Warning, TEXT("isFound = %d"), isFound);

	// 敵に捕まったら
	if (isFound)
	{
		// 敵のアニメーション終了までRespawn関数は呼ばない

		// リスポーン関数を呼び出し
		Respawn();
	}
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
	

	// VR's turning by_Rin
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
	
		if (UHeadMountedDisplayFunctionLibrary::HasValidTrackingPosition())
		{
			FRotator HMDRotation;
			FVector HMDLocation;
			UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);

			// need to test these two
			// newRotationPlayer.Yaw = HMDRotation.Yaw;
			newRotationPlayer.Yaw = ( HMDRotation.Yaw - newRotationPlayer.Yaw) *_deltaTime ;
		} // end if()
		else
		{ 
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Red, TEXT("Can't Track the HMD In [UpdateCamera__Yaw]"));

		} // end else

	} // end if()
	else
	{
		// Yaw(プレイヤーを回転させる)		masui
		newRotationPlayer.Yaw += m_cameraRotateInput.X * m_cameraRotateSpeed * _deltaTime;

	}

	// プレイヤーに回転情報を設定
	SetActorRotation(newRotationPlayer);
}

// プレイヤーの移動処理
void APlayerCharacter::UpdatePlayerMove(const float _deltaTime)
{
	// ベクトルの長さを取得
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Yellow, FString::SanitizeFloat(vectorLength));
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("vel: %s"), *GetCharacterMovement()->Velocity.ToString()));
	GetCharacterMovement()->MaxAcceleration = 400.f;

	// 移動量を決定
	// 走る
	if (vectorLength >= 0.5f)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
		m_playerMoveSpeed = m_playerRunSpeed;
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}
	// 歩く
	else if (vectorLength > 0.0f && vectorLength < 0.5f)
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
		m_playerMoveSpeed = m_playerWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;


	}
	// 止まる
	else
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
		m_playerMoveSpeed = 0.0f;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}

	// しゃがんでいた場合移動速度を1/2に
	if (!m_isStanding)
	{
		m_playerMoveSpeed /= 2.0f;
	}

	// ベクトルの正規化
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	//m_playerMoveSpeed *= _deltaTime;

	// player's speed
	 GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Purple, FString::SanitizeFloat(GetCharacterMovement()->Velocity.Size()));

	

	// 地面との距離を測りプレイヤーの高さを設定
	SetEyeLevel(_deltaTime, (m_playerMoveSpeed* _deltaTime));


	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		AddMovementInput(GetActorForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(GetActorRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));

	} // end if()
	else
	{
		// masui
		AddMovementInput(GetActorForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(GetActorRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));
	} // end else
}

// 地面との距離を測りプレイヤーの高さを設定
void APlayerCharacter::SetEyeLevel(const float _deltaTime, const float _player_move_speed)
{
	// プレイヤーの歩行による視線の縦揺れ
	eyelevel_for_camera_shaking = ReturnCameraVerticalShaking(_deltaTime, _player_move_speed);

	// 立っていればそのまましゃがんでいればアイレベルを1/2にして座標セット
	if (m_isStanding)
	{
		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, (m_eyeLevelWhenStanding + eyelevel_for_camera_shaking)));
	}
	else
	{
		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, ((m_eyeLevelWhenStanding * 0.5f) + eyelevel_for_camera_shaking)));
	}
}

// プレイヤーの歩行による視線の縦揺れ
float APlayerCharacter::ReturnCameraVerticalShaking(const float _deltaTime, const float _player_move_speed)
{
	// 移動していなければデフォルトの目の高さへ移動する
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
	// 移動していれば移動量を加算
	else
	{
		count_for_footstep_ += _player_move_speed;
	}

	if (count_for_footstep_ >= player_footstep_span_ && can_make_footstep)
	{
		// 足音を立てる、許可フラグを下す
		MakeFootstep(_deltaTime, _player_move_speed);
		can_make_footstep = false;
	}

	// 範囲外ならリセット、足音許可フラグを立てる
	if (count_for_footstep_ >= player_footstep_span_ * 2.0f || count_for_footstep_ < 0.0f)
	{
		count_for_footstep_ = 0.0f;
		can_make_footstep = true;
	}

	float eyelevel = count_for_footstep_ * (180.0f / player_footstep_span_);
	eyelevel = FMath::DegreesToRadians(eyelevel);
	eyelevel = FMath::Cos(eyelevel);

	// 補正
	eyelevel += -1.0f;	// 0〜-2
	eyelevel *= 0.5f;	// 0〜-1
	eyelevel *= camera_shaking_value;	// 0〜camera_shaking_value

	return eyelevel;
}

// 足音を鳴らす
void APlayerCharacter::MakeFootstep(const float _deltaTime, const float _player_move_speed)
{
	// 音量調節
	float volume = _player_move_speed / (m_playerRunSpeed * _deltaTime);

	// 再生
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_player_footstep_, GetActorLocation(), volume, 1.0f, 0.0f);

	// MakeNoise()
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
					// 白枠を非表示にする	by	朱適
					m_pPrevCheckItem->SetOutline(false);

					// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
					OnItemCheckEndEventDispatcher.Broadcast();
				}
				// 新しくチェックしたオブジェクトの被チェックを有効に
				m_pCheckingItem->m_isChecked = true;
				// 白枠を表示にする		by	朱適
				m_pCheckingItem->SetOutline(true);

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
			// 白枠を非表示にする	by	朱適
			m_pPrevCheckItem->SetOutline(false);
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


// ===========  VR HMDのリセット  by_Rin ===========
void APlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
} // void APlayerCharacter::OnResetVR()

// =====  VR Motion コントローラー ポインターの関数  by_Rin =====
void APlayerCharacter::UpdateVRLaser()
{
	FVector StartPoint = FVector::ZeroVector;			// Laserの初期位置
	FVector FowardActor = FVector::ZeroVector;			// モーションコントローラーの前方向

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

	FVector EndPoint = ((FowardActor * LASERLENGTH) + StartPoint);				// Laserの終点

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// テスト用ポインター
	// DrawDebugLine (GetWorld (), StartPoint, EndPoint, FColor::Red, false, 1.0f);

	ULineBatchComponent* const LineBatcher = GetWorld()->PersistentLineBatcher;

	// Laserが最初に当たったのものを　vr_HitResult　に反映する
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

// リスポーン関数(作成者：尾崎)
void APlayerCharacter::Respawn()
{
	//--- ゲームオーバーに遷移の追加 by 朱適
	if (GetWorld()->GetName() == "ProtoType")
	{
		LevelSwitchHelper::OpenGameOverLevelMap(this);
	}

	// フラグの初期化
	isFound = false;

	//// デバッグ確認用(死んだときのログ)
	//UE_LOG(LogTemp, Warning, TEXT("Player Dead... Respawn."));

	// 位置の初期化
	this->SetActorLocation(FVector(-4850.f, -3300.f, 300.f));
}
