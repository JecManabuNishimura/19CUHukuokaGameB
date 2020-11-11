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
//				：2020/10/14		白枠の制御の追加
//				：2020/11/04		心拍数アプリのアクションマッピングを追加
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.cpp
// 概要			：VRモーションの対応
// 作成者		：19CU0236 林雲暉
// 作成日		：2020/08/28		VRモーションコントローラーの対応
// 更新日		：2020/09/06		VRのrayの作成
//				：2020/09/19		VRのスマートフォン作成
//				：2020/10/20		Player移動としゃがむの調整
//								スプリングアームの追加
//				：2020/10/28		VRの移動としゃがむの調整
//				：2020/11/05		PC版スマホを手前に持つ方法変更
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
#include "GameFramework/Actor.h" // makeNoise用


#define LASERLENGTH 300.0f		// VR用 LASERの長さ(手の長さを代表すること) by_Rin

// コンストラクタ
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
 	// ティックを呼び出すかのフラグ
	PrimaryActorTick.bCanEverTick = true;

	// デフォルトプレイヤーとして設定
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// スプリングアームのオブジェクトを生成
	m_pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("m_pSpringArm"));

	// スプリングアームにRootComponentをアタッチ
	m_pSpringArm->SetupAttachment(RootComponent);

	// カメラとプレイヤーの距離
	m_pSpringArm->TargetArmLength = 0.f;

	// カメラの子リジョンテストを行うかを設定
	m_pSpringArm->bDoCollisionTest = false;

	// カメラ追従ラグを使うかを設定
	m_pSpringArm->bEnableCameraLag = true;

	// カメラ追従ラグの速度を設定
	m_pSpringArm->CameraLagSpeed = 20.f;

	// カメラ原点の生成
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// カメラを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// カメラ原点にスプリングアームをアタッチ
	m_pCameraBase->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);	

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

	// ===========  プレイヤー移動、しゃがむ用のプロパティ設定  by_Rin ===========
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;					// しゃがむを可能します
	GetCharacterMovement()->CrouchedHalfHeight = 40.f;							// しゃがむ時の高さ
	GetCharacterMovement()->bShrinkProxyCapsule = true;							// しゃがむ時のcollisionの変更を可能します(必要ないかも)
	GetCharacterMovement()->MaxWalkSpeedCrouched = m_MaxWalkSpeed_Crouch;		// しゃがむ時の移動速度
	GetCharacterMovement()->MaxAcceleration = 500.f;							// プレイヤー移動の加速度

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

			/*
			bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// pathにあるクラスを取得
			if (bp_VRphone != nullptr)
			{
				vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VRのスマートフォンをActorとして生成する

				vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
				vr_Phone->SetActorEnableCollision(false);

				if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
				{
					// VR用配置
					// スマホ他の方向、先に確認しました (仮(メッシュの初期方向対応め)Y X Z)
					vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ↑

					// vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||

					//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

					//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ↑display
																									//   ||
					vr_Phone->SetActorRelativeLocation(FVector(200, 0, 10));

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

	// 仮用PCスマホ
	// Epic Comment :D // Spawn and attach both motion controllers
	const FTransform SpawnTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)); // = FTransform::Identity;
	FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	bp_VRphone = TSoftClassPtr<AActor>(FSoftObjectPath(*path)).LoadSynchronous();	// pathにあるクラスを取得
	if (bp_VRphone != nullptr)
	{
		vr_Phone = GetWorld()->SpawnActor<AActor>(bp_VRphone);						// VRのスマートフォンをActorとして生成する


		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
		{
			vr_Phone->AttachToComponent(LeftController->GetRootComponent()->GetChildComponent(0), AttachRules);
			vr_Phone->SetActorEnableCollision(false);

			// VR用配置
			// スマホ他の方向、先に確認しました (仮(メッシュの初期方向対応め)Y X Z)
			vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ↑

			// vr_Phone->SetActorRelativeRotation(FRotator( 0.f, -180.f, -90.f));		//   display <- ||

			//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, -90.f));		//   || -> display

			//  vr_Phone->SetActorRelativeRotation(FRotator(180.f, 0.f, 0.f));			//   ↑display
																							//   ||
			vr_Phone->SetActorRelativeLocation(FVector(200, 0, 10));

			// VRスマホのサイズ
			vr_Phone->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));

			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
		} // end if
		else
		{
			vr_Phone->AttachToComponent(m_pCamera, AttachRules);
			vr_Phone->SetActorEnableCollision(false);

			// PC確認用配置
			vr_Phone->SetActorRelativeRotation(FRotator(-90.f, -180.f, 180.f));
			vr_Phone->SetActorRelativeLocation(FVector(200.f, -150.f, -50.f));

			// PCスマホのサイズ
			vr_Phone->SetActorScale3D(FVector(0.4f, 0.4f, 0.4f));

			vr_Phone->SetActorHiddenInGame(true);

		} // end else

		// 確認用
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetName()));

	} // end if()

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

	// スマホを構える・構えを解除(作成者：尾崎)　今は使いている by_Rin
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);

	// 心拍数アプリの切り替え
	InputComponent->BindAction("HeartBeatStatusSwitch", IE_Pressed, this, &APlayerCharacter::HeartBeatStatusSwitch);
}

// カメラ(Pitch)の更新
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
	// 操作不可ならreturn
	if (!can_player_control)	return;

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
	// 操作不可ならreturn
	if (!can_player_control)	return;

	// 現在のプレイヤーの回転情報を取得
	FRotator newRotationPlayer = GetActorRotation();
	
	// VR's 回転 by_Rin
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// 何もしない
			
		/*
		// Rotation確認用
		FRotator HMDRotation;
		FVector HMDLocation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(HMDRotation, HMDLocation);
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::SanitizeFloat(HMDRotation.Quaternion().Z));
		*/

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
	// 操作不可ならreturn
	if (!can_player_control)	return;

	// ベクトルの長さを取得
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// 移動量を決定		修正by_Rin
	if (m_isStanding == true && (GetCharacterMovement()->IsCrouching() == false))
	{
		/*
		// unreal engine's bug, dont use it. Using BluePrint Now.
			GetCharacterMovement()->UnCrouch(true);
		*/

		// 走る
		if (vectorLength >= 0.5f)
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Running"));
			m_playerMoveSpeed = m_playerRunSpeed;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Run;
		}
		// 歩く
		else if (vectorLength > 0.0f && vectorLength < 0.5f)
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Walking"));
			m_playerMoveSpeed = m_playerWalkSpeed;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Walk;
		}
		// 止まる
		else
		{
			GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Stop"));
			m_playerMoveSpeed = 0.0f;
			GetCharacterMovement()->MaxWalkSpeed = m_MaxWalkSpeed_Walk;
		}
	} // end if()
	// しゃがんでいた場合移動速度を1/2に
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

	// ベクトルの正規化
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// プレイヤーの移動速度確認用
	GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Purple, FString::SanitizeFloat(GetCharacterMovement()->Velocity.Size()));

	// 地面との距離を測りプレイヤーの高さを設定
	SetEyeLevel(_deltaTime, (m_playerMoveSpeed * _deltaTime));

	
	// VR時の移動 by_Rin
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == true)
	{
		// VR's HMD rotation.z maybe is Radian, its between -1 ~ 1
		// so use VR camera's Rotation

		AddMovementInput(m_pCamera->GetForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(m_pCamera->GetRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));

	} // end if()
	// PC時の移動
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("=== %s"), *GetActorForwardVector().ToString()));

		// PC時の移動
		AddMovementInput(GetActorForwardVector(), (m_playerMoveSpeed * m_playerMoveInput.X));
		AddMovementInput(GetActorRightVector(), (m_playerMoveSpeed * m_playerMoveInput.Y));
	} // end else
}


void APlayerCharacter::PlayerStand() {
	// C++のしゃがむが使えないため, コメントアウトしました（BluePrintに設定している）  by_Rin
	// m_isStanding = true;
}

// プレイヤーアクション：しゃがむ
void APlayerCharacter::PlayerSquat() {
	// C++のしゃがむが使えないため, コメントアウトしました（BluePrintに設定している）	by_Rin
	// m_isStanding = false;
}

// 地面との距離を測りプレイヤーの高さを設定
void APlayerCharacter::SetEyeLevel(const float _deltaTime, const float _player_move_speed)
{
	// プレイヤーの歩行による視線の縦揺れ
	eyelevel_for_camera_shaking = ReturnCameraVerticalShaking(_deltaTime, _player_move_speed);

	// 立っていればそのまましゃがんでいればアイレベルを1/4にして座標セット
	// しゃがむと合わせての条件を入れった	by_Rin
	if (m_isStanding && (GetCharacterMovement()->IsCrouching() == false))
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Stand eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, ( m_eyeLevelWhenStanding + eyelevel_for_camera_shaking)));

		// PC しゃがむの時　スマホ位置の調整
		if (holdingSmartphoneState == 1)
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
			{
				vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, -2.f));			// PC用のスマホ配置
			
			} // end if()
		} // end if()
		
	}
	else
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("Crouch eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, ((m_eyeLevelWhenStanding / 4) + eyelevel_for_camera_shaking)));
		

		// PC しゃがむの時　スマホ位置の調整
		if (holdingSmartphoneState == 1)
		{
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
			{
				vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, 0.f));			// PC用のスマホ配置
			} // end if()
		} // end if()
		
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
	eyelevel += -1.0f;	// 0～-2
	eyelevel *= 0.5f;	// 0～-1
	eyelevel *= camera_shaking_value;	// 0～camera_shaking_value

	return eyelevel;
}

// 足音を鳴らす
void APlayerCharacter::MakeFootstep(const float _deltaTime, const float _player_move_speed)
{
	// 音量調節
	float volume = _player_move_speed / (m_playerRunSpeed * _deltaTime);
	
	// 再生
	if (sound_player_footstep_ != NULL)UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_player_footstep_, GetActorLocation(), volume, 1.0f, 0.0f);

	MakeNoise(volume, this, this->GetActorLocation(), 1.0f);
}

void APlayerCharacter::CheckItem()
{
	// 操作不可なら表示されているコマンドアイコンを非表示にし、return
	if (!can_player_control)
	{
		// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
		OnItemCheckEndEventDispatcher.Broadcast();

		return;
	}

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
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Purple, TEXT("  X  "));

	m_playerMoveInput.X = _axisValue;
}
// プレイヤー移動：移動Y軸方向(横)
void APlayerCharacter::PlayerMoveY(float _axisValue)
{
	GEngine->AddOnScreenDebugMessage(10, 5.0f, FColor::Purple, TEXT("  Y  "));
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

// PC版、スマホを手前に持っているか  by_Rin
void APlayerCharacter::ChangeHaveSmartphoneFlag()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// スマホをポケットにしまう
		if (holdingSmartphoneState == 2)
		{
			holdingSmartphoneState = 0;
			vr_Phone->SetActorHiddenInGame(true);

			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Is Not Holding Smartphone");
		} // end if()
		// スマホを目前に持つ
		else if (holdingSmartphoneState == 1)
		{
			holdingSmartphoneState = 2;
			vr_Phone->SetActorRelativeLocation(FVector(10.f, 0.f, 0.f));
			vr_Phone->SetActorHiddenInGame(false);
			vr_Phone->SetActorScale3D(FVector(0.06f, 0.06f, 0.06f));						// PCスマホのサイズ

			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Looking at Smartphone");
		} // end else
		// スマホを手前に持つ
		else
		{
			holdingSmartphoneState = 1;
			vr_Phone->SetActorRelativeLocation(FVector(10.f, -6.f, -2.f));
			vr_Phone->SetActorHiddenInGame(false);
			vr_Phone->SetActorScale3D(FVector(0.02f, 0.02f, 0.02f));						// PCスマホのサイズ

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

// C++のしゃがむが使えないため
// CheckStandingVRも仮放棄
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