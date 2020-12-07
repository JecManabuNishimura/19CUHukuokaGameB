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
// 更新日		：2020/11/04		心拍数アプリのアクションマッピングを追加
// 更新日		：2020/11/16		ダメージ状態２を追加
// 更新日		：2020/11/17		ダメージ状態１、２の処理を追加
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
//				：2020/11/25		VRモードの時スマホ使えるようにに設定する
//				：2020/11/29		VRのCheckItem追加
//-------------------------------------------------------------------

#include "PlayerCharacter.h"
#include "Engine.h"				// GEngineを呼び出すためのヘッダ
#include "SteamVRChaperoneComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "VirtualReality/ThrillerVR_MotionController.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
// 以上の6点はVR用インクルード (作成者:林雲暉)
#include "LevelSwitchHelper.h"	// マップ遷移用クラス
#include "GameFramework/Actor.h" // makeNoise用
#include "Containers/StringConv.h"	// 文字数をカウント用


#define LASERLENGTH 250.0f		// VR用 LASERの長さ(手の長さを代表すること) (作成者:林雲暉)

// コンストラクタ
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
	m_pSpringArm->bEnableCameraLag = false;

	// カメラ追従ラグの速度を設定
	m_pSpringArm->CameraLagSpeed = 35.f;

	// カメラ原点の生成
	m_pCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));

	// カメラを生成
	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("m_pCamera"));

	// カメラ原点にスプリングアームをアタッチ
	m_pCameraBase->SetupAttachment(m_pSpringArm, USpringArmComponent::SocketName);

	// カメラ原点にカメラをアタッチ
	m_pCamera->SetupAttachment(m_pCameraBase);

	// ダメージ演出用浮動小数点カーブ取得
	const ConstructorHelpers::FObjectFinder<UCurveFloat> curve_for_damage_effect(TEXT("CurveFloat'/Game/Curve/DamageEffect.DamageEffect'"));

	damage_effect_timeline_ = FTimeline{};

	// ダメージを受けた際の赤くなるエフェクト更新処理のバインド
	FOnTimelineFloat timeline_update;
	timeline_update.BindUFunction(this, "TimelineUpdate");
	damage_effect_timeline_.AddInterpFloat(curve_for_damage_effect.Object, timeline_update);


	// ダメージを受けた際の赤くなるエフェクト終了処理のバインド
	FOnTimelineEvent timeline_finish;
	timeline_finish.BindUFunction(this, "TimelineFinish");
	damage_effect_timeline_.SetTimelineFinishedFunc(timeline_finish);
}

// デストラクタ
APlayerCharacter::~APlayerCharacter()
{
}

// 実行時に一度呼ばれる
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GEngine->FixedFrameRate = 60.f;

	// 効果音の音量を初期化(=1に)
	if (se_volume_can_change_ != NULL)	se_volume_can_change_->Properties.Volume = 1.f;

	// ボックストレースの描画設定
	if (draw_debug_trace_)	draw_debug_trace_type_ = EDrawDebugTrace::ForOneFrame;

	// ===========  プレイヤー移動、しゃがむ用のプロパティ設定  (作成者:林雲暉) ===========
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;					// しゃがむを可能します
	GetCharacterMovement()->CrouchedHalfHeight = 40.f;							// しゃがむ時の高さ
	GetCharacterMovement()->bShrinkProxyCapsule = true;							// しゃがむ時のcollisionの変更を可能します(必要ないかも)
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

	// ===========  VR Motion Controller's Spawn and Attach  (作成者:林雲暉) ===========
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
			LeftController->GetMotionController()->bDisableLowLatencyUpdate = true;

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
					vr_Phone->SetActorRelativeRotation(FRotator(270.f, 0.f, 0.f));			//  ↑1

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
			RightController->GetMotionController()->bDisableLowLatencyUpdate = true;
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
			vr_Phone->SetActorRelativeLocation(FVector(100, 0, 10));

			// VRスマホのサイズ
			vr_Phone->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));

			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("===== %s"), *vr_Phone->GetActorRotation().ToString()));
			
			holdingSmartphoneState = 1;
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

	// ===========  VR Motion Controller's Laser Update (作成者:林雲暉) ===========
	// 今はVRモード?
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		// アイテムのチェック
		CheckItem();
	}
	// ===========  VR Motion Controller's Laser Update (作成者:林雲暉) ===========
	else 
	{
		// VR コントローラー ポインターの更新
		// VRのアイテムのチェック
		UpdateVRLaser();
	} // end if()

	// ダメージ状態ならTimeLine更新
	if (is_damaged_)	damage_effect_timeline_.TickTimeline(DeltaTime);
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

	// スマホを構える・構えを解除(作成者：尾崎)　今は使いている (作成者:林雲暉)
	InputComponent->BindAction("HaveSmartphone", IE_Pressed, this, &APlayerCharacter::ChangeHaveSmartphoneFlag);
	InputComponent->BindAction("Smartphone_Light", IE_Pressed, this, &APlayerCharacter::ChangeLightFlag);
	InputComponent->BindAction("Smartphone_Shutter", IE_Pressed, this, &APlayerCharacter::ChangeShutterFlag);

	// 心拍数アプリの切り替え
	// VRコントロールに対応するため統一になった(作成者:林雲暉)
	InputComponent->BindAction("UseSmartPhone", IE_Pressed, this, &APlayerCharacter::HeartBeatStatusSwitch);
}

// カメラ(Pitch)の更新
void APlayerCharacter::UpdateCameraPitch(const float _deltaTime)
{
	// 操作不可ならreturn
	if (!can_player_camera_control_)	return;

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
	if (!can_player_camera_control_)	return;

	// 現在のプレイヤーの回転情報を取得
	FRotator newRotationPlayer = GetActorRotation();
	
	// VR's 回転 (作成者:林雲暉)
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
	if (!can_player_move_control_ || in_the_locker_)	return;

	// ベクトルの長さを取得
	float vectorLength = ReturnVector2DLength(&m_playerMoveInput);

	// 移動量を決定		(修正者:林雲暉)(12/06 仕様書通りの挙動をしていないため再修正 修正者:増井悠斗)
	/*
	// unreal engine's bug, dont use it. Using BluePrint Now.
		GetCharacterMovement()->UnCrouch(true);
	*/

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

	// 立っている時の速度設定
	GetCharacterMovement()->MaxWalkSpeed = m_playerMoveSpeed;

	// しゃがんでいた場合設定した速度を1/2に
	if (m_isStanding == false && (GetCharacterMovement()->IsCrouching() == true))
	{
		GEngine->AddOnScreenDebugMessage(10, _deltaTime, FColor::Green, TEXT("PlayerMoveState : Squat"));
		m_playerMoveSpeed /= 2.0f;

		// しゃがんでいる時の速度設定
		// GetCharacterMovement()->MaxWalkSpeedCrouched = m_playerMoveSpeed;

		/*
		// unreal engine's bug, dont use it. Using BluePrint Now.
		if (GetCharacterMovement()->IsCrouching() == false)
		{
			GetCharacterMovement()->Crouch(true);
		} // end if
		*/
	}

	// ベクトルの正規化
	NormalizedVector2D(vectorLength, &m_playerMoveInput);

	// プレイヤーの移動速度確認用
	GEngine->AddOnScreenDebugMessage(30, 10.0f, FColor::Purple, FString::SanitizeFloat(GetCharacterMovement()->Velocity.Size()));

	// 地面との距離を測りプレイヤーの高さを設定
	SetEyeLevel(_deltaTime, (m_playerMoveSpeed * _deltaTime));

	// VR時の移動 (作成者:林雲暉)
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
	// C++のしゃがむが使えないため, コメントアウトしました（BluePrintに設定している）  (作成者:林雲暉)
	// m_isStanding = true;
}

// プレイヤーアクション：しゃがむ
void APlayerCharacter::PlayerSquat() {
	// C++のしゃがむが使えないため, コメントアウトしました（BluePrintに設定している）	(作成者:林雲暉)
	// m_isStanding = false;
}

// 地面との距離を測りプレイヤーの高さを設定
void APlayerCharacter::SetEyeLevel(const float _deltaTime, const float _player_move_speed)
{
	// プレイヤーの歩行による視線の縦揺れ
	eyelevel_for_camera_shaking = ReturnCameraVerticalShaking(_deltaTime, _player_move_speed);

	// 立っていればそのまましゃがんでいればアイレベルを1/4にして座標セット
	// しゃがむと合わせての条件を入れった	(作成者:林雲暉)
	if (m_isStanding && (GetCharacterMovement()->IsCrouching() == false))
	{
		//	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, FString::Printf(TEXT("Stand eye Loc: %s"), *m_pCamera->GetRelativeLocation().ToString()));
		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f,  eyelevel_for_camera_shaking));

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

		m_pCamera->SetRelativeLocation(FVector(0.0f, 0.0f, eyelevel_for_camera_shaking));
		
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
	if (!can_player_move_control_ || isFound || in_the_locker_)
	{
		if (m_pPrevCheckItem != NULL)
		{
			// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
			OnItemCheckEndEventDispatcher.Broadcast();
			m_pPrevCheckItem->SetOutline(false);
		}
		return;
	}

	// トレースに必要な変数を宣言
	FHitResult outHit;

	// 現在位置を取得
	FVector start = m_pCamera->GetComponentLocation() + (m_pCamera->GetForwardVector() * 40.f);
	FVector end = start + (m_pCamera->GetForwardVector() * check_to_actor_trace_length_);

	// 1フレーム前のアイテムの情報を移す
	m_pPrevCheckItem = m_pCheckingItem;

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);

	// ライントレースからボックストレースに変更(11/20 増井)
	if(UKismetSystemLibrary::BoxTraceSingle(this, start, end, box_half_size_, GetActorRotation(), TraceTypeQuery3, false, actors_to_ignore, draw_debug_trace_type_, outHit, true, FLinearColor::Red, FLinearColor::Green, 1.0f))
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
		else
		{
			if (m_pPrevCheckItem != NULL)
			{
				// 前フレームでチェックしていたオブジェクトの被チェックを無効に
				m_pPrevCheckItem->m_isChecked = false;

				// 白枠を非表示にする	by	朱適
				m_pPrevCheckItem->SetOutline(false);

				// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
				OnItemCheckEndEventDispatcher.Broadcast();
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

// ダメージを受けた際の赤くなるエフェクト更新処理
void APlayerCharacter::TimelineUpdate(float value)
{
	m_pCamera->PostProcessSettings.SceneColorTint = FVector(1.f, 1.f - value, 1.f - value);
	m_pCamera->PostProcessSettings.VignetteIntensity = value;
}

// ダメージを受けた際の赤くなるエフェクト終了処理
void APlayerCharacter::TimelineFinish()
{
	m_pCamera->PostProcessSettings.SceneColorTint = FVector(1.f, 1.f, 1.f);
	m_pCamera->PostProcessSettings.VignetteIntensity = 0.f;

	is_damaged_ = false;
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

void APlayerCharacter::AttackFromEnemy()
{
	++damage_count_;

	is_damaged_ = true;
	damage_effect_timeline_.PlayFromStart();

	switch (damage_count_)
	{
	case 1:
		// ダメージ状態２になる（作成者：朱適）
		EarDamaged();

		// 被ダメージ1回目(聴覚デバフ/効果音のサウンドクラスを通して音量の設定)
		if (se_volume_can_change_ != NULL)	se_volume_can_change_->Properties.Volume = se_volume_for_debuff_;
		else								GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SE_VolumeCanChange is not set.");
		break;
	case 2:
		// ダメージ状態２になる（作成者：朱適）
		HandDamaged();
		break;
	case 3:
		// ダメージ状態３になる（作成者：朱適）
		EyeDamaged();

		// 被ダメージ3回目(視覚デバフ/ポストプロセスの値設定)
		m_pCamera->PostProcessSettings.ColorSaturation = saturation_for_debuff_;
		m_pCamera->PostProcessSettings.ColorContrast = contrast_for_debuff_;
		m_pCamera->PostProcessSettings.VignetteIntensity = vignette_intensity_for_debuff_;
		m_pCamera->PostProcessSettings.GrainIntensity = grain_intensity_for_debuff_;
		m_pCamera->PostProcessSettings.FilmSlope = film_slope_for_debuff_;
		m_pCamera->PostProcessSettings.FilmToe = film_toe_for_debuff_;
		break;
	case 4:
		// 被ダメージ4回目(死:リスポーン)
		Respawn();
		break;
	default:
		break;
	}
}

// エネミーの攻撃範囲に入った際の処理 (追記者 増井)
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

// PC版、スマホを手前に持っているか  (作成者:林雲暉)
void APlayerCharacter::ChangeHaveSmartphoneFlag()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() == false)
	{
		if (in_the_locker_ == false)
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

// _missionIDというミッションのフラグと表示を処理する	(作成者:林雲暉)
// utilityやセーブデータのミッションフラグも処理するを入れで方がいいです。
// _isDeleteはtrueの時ミッションを画面から削除する。
// falseの時ミッションを追加する
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

// 文字をワイドBYTEに変換	(作成者:林雲暉)
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


// ダメージ状態の処理（C++側）（作成者：朱適）
void APlayerCharacter::EarDamaged_Implementation()
{
}
void APlayerCharacter::HandDamaged_Implementation()
{
}
void APlayerCharacter::EyeDamaged_Implementation()
{
}

// カメラの座標を返す
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

// ロッカーにいるかどうかのフラグセット関数(厳密にいうと入ろうとするところから完全に出るまで)(作成者：尾崎)
void APlayerCharacter::SetInTheLocker(const bool flag)
{
	in_the_locker_ = flag;
	// スマホを使えない判定追加(作成者:林雲暉)
	if (in_the_locker_ == true && holdingSmartphoneState != 0)
	{
		ChangeHaveSmartphoneFlag();
	} // end if()

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


// ===========  VR HMDのリセット  (作成者:林雲暉) ===========
void APlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
} // void APlayerCharacter::OnResetVR()

// =====  VR Motion コントローラー ポインターの関数  (作成者:林雲暉) =====
void APlayerCharacter::UpdateVRLaser()
{
	// ===============================================================
	// 11/29	VR CheckItem追加
	// 12/01	トレースタイプ変更
	// ===============================================================

	FVector StartPoint = FVector::ZeroVector;			// Laserの初期位置
	FVector FowardActor = FVector::ZeroVector;			// モーションコントローラーの前方向

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

	FVector EndPoint = ((FowardActor * LASERLENGTH) + StartPoint);				// Laserの終点

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// テスト用ポインター
	// DrawDebugLine (GetWorld (), StartPoint, EndPoint, FColor::Red, false, 1.0f);

	ULineBatchComponent* const LineBatcher = GetWorld()->PersistentLineBatcher;


	// 操作不可なら表示されているコマンドアイコンを非表示にし、return
	if (!can_player_move_control_ || isFound || in_the_locker_)
	{
		if (m_pPrevCheckItem != NULL)
		{
			// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
			OnItemCheckEndEventDispatcher.Broadcast();
			m_pPrevCheckItem->SetOutline(false);
		}
		return;
	}

	// 1フレーム前のアイテムの情報を移す
	m_pPrevCheckItem = m_pCheckingItem;

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);
	actors_to_ignore.Add(LeftController);
	actors_to_ignore.Add(RightController);
	actors_to_ignore.Add(vr_Phone);


	// Laserが最初に当たったのものを　vr_HitResult　に反映する
	// if (GetWorld()->LineTraceSingleByChannel(vr_HitResult, StartPoint, EndPoint, ECC_WorldStatic, CollisionParams))
	if (UKismetSystemLibrary::BoxTraceSingleByProfile(RightController, StartPoint, EndPoint, (box_half_size_/2), RightController->GetMotionController()->K2_GetComponentRotation(), FName("RightContollerCheckCollision"), false, actors_to_ignore, EDrawDebugTrace::ForOneFrame, vr_HitResult, true,FLinearColor::Green,FLinearColor::Red, 0.1f))
	{

		// アイテム基本クラスにキャスト
		m_pCheckingItem = Cast<AItemBase>(vr_HitResult.GetActor());

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
				// 前フレームでチェックしていたオブジェクトの被チェックを無効に
				m_pPrevCheckItem->m_isChecked = false;

				// 白枠を非表示にする	by	朱適
				m_pPrevCheckItem->SetOutline(false);

				// イベントディスパッチャー呼び出し(アイテムコマンドUIをビューポートから消す)
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

	} // end else

} // APlayerCharacter::UpdateVRLaser()

// リスポーン関数(作成者：尾崎)
void APlayerCharacter::Respawn()
{
	//--- ゲームオーバーに遷移の追加 by 朱適
	LevelSwitchHelper::OpenGameOverLevelMap(this);
}

// C++のしゃがむが使えないため
// CheckStandingVRも仮放棄
// (作成者:林雲暉)
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