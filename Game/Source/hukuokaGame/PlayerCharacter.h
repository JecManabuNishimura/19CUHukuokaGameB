//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.h
// 概要			：プレイヤーキャラを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/07
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.h
// 概要			：VRカメラの作成
// 作成者		：19CU0217 朱適
// 作成日		：2020/08/18
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.h
// 概要			：VRモーションの対応
// 作成者		：19CU0236 林雲暉
// 作成日		：2020/08/28
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Smartphone.h"
#include "Components/ChildActorComponent.h"
#include "PlayerCharacter.generated.h"

// 前方宣言
class AItemBase;

// イベントディスパッチャー宣言
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemCheckBeginEventDispatcher);		// プレイヤーの視線がCanCheckアイテムに当たった時
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemCheckEndEventDispatcher);			// プレイヤーの視線がCanCheckアイテムから外れた時

UCLASS()
class HUKUOKAGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// コンストラクタ
	APlayerCharacter();
	// デストラクタ
	~APlayerCharacter();

protected:
	// 実行時に一度呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void Tick(float DeltaTime) override;

	// 各入力関係メソッドとのバインド処理
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// =====  VR Motion コントローラー 関数  by_Rin =====
	// Resets HMD orientation and position in VR
	void OnResetVR();

private:
	// カメラ(Pitch)の更新
	void UpdateCameraPitch(const float _deltaTime);

	// カメラ(Yaw)の更新
	void UpdateCameraYaw(const float _deltaTime);

	// プレイヤーの移動処理
	void UpdatePlayerMove(const float _deltaTime);

	// 地面のとの距離を測りプレイヤーの高さを設定
	void SetEyeLevel(const float _deltaTime, const float _player_move_speed);

	// プレイヤーの歩行による視線の縦揺れ
	float ReturnCameraVerticalShaking(const float _deltaTime, const float _player_move_speed);

	// 足音を鳴らす
	void MakeFootstep(const float _deltaTime, const float _player_move_speed);

	// アイテムチェック
	void CheckItem();

	// ベクトルの長さを返す
	float ReturnVector2DLength(const FVector2D* _pFvector2d);

	// ベクトルを正規化する
	void NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d);

	// =====  VR Motion コントローラー ポインターの関数  by_Rin =====
	void UpdateVRLaser();

private:
	// 入力バインド
	// カメラ回転：Pitch(Y軸)
	void CameraRotatePitch(float _axisValue);
	// カメラ回転：Yaw(Z軸)
	void CameraRotateYaw(float _axisValue);

	// プレイヤー移動：移動X軸方向(縦)
	void PlayerMoveX(float _axisValue);
	// プレイヤー移動：移動Y軸方向(横)
	void PlayerMoveY(float _axisValue);

	// プレイヤーアクション：立ちあがる
	void PlayerStand() { m_isStanding = true; }
	// プレイヤーアクション：しゃがむ
	void PlayerSquat() { m_isStanding = false; }

	// プレイヤーアクション：拾う、調べる、作動させる
	void CheckToActor();

	// スマホ関係
	// 構えるかどうかのフラグ(作成者：尾崎)
	void ChangeHaveSmartphoneFlag()
	{
		isHaveSmartphoneFlag = !isHaveSmartphoneFlag;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Push HaveSmartphone");
	};
	// ライトをつけるかどうか(作成者：尾崎)
	void ChangeLightFlag()
	{
		lightFlag = !lightFlag;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Push Smartphone_Light");

	};
	// シャッターを切るかどうか(作成者：尾崎)
	void ChangeShutterFlag()
	{
		shatterFlag = !shatterFlag;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, "Push Smartphone_Shutter");
	};

public:
	// Smartphoneから呼び出す関数(isHaveSmartphoneFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetisHaveSmartphoneFlag();

	// Smartphoneから呼び出す関数(lightFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetLightFlag();

	// Smartphoneから呼び出す関数(shutterFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetShatterFlag();

public:
	UFUNCTION(BlueprintCallable, Category = "Return State")
		AItemBase* ReturnCheckingItem() const;

	UFUNCTION(BlueprintCallable, Category = "Return State")
		FString ReturnCheckingItemCommandName() const;

private:
	// イベントディスパッチャー定義
	// プレイヤーの視線がCanCheckアイテムに当たった時
	UPROPERTY(BlueprintAssignable)
		FOnItemCheckBeginEventDispatcher  OnItemCheckBeginEventDispatcher;

	// プレイヤーの視線がCanCheckアイテムから外れた時
	UPROPERTY(BlueprintAssignable)
		FOnItemCheckEndEventDispatcher  OnItemCheckEndEventDispatcher;

	// プロパティ
	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerThresholdToRun;			// 走る閾値(0 < this <= 1)

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerRunSpeed;					// プレイヤーの歩く速度

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerWalkSpeed;				// プレイヤーの走る速度

	UPROPERTY(EditAnyWhere, Category = "Move")
		float player_footstep_span_;			// プレイヤーの足音の間隔(プレイヤーの速度 / 間隔 = 1秒間に鳴る足音の数)

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMin;			// カメラのPitch制限最小角度

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMax;			// カメラのPitch制限最大角度

	UPROPERTY(EditAnywhere, Category = "Camera")
		class USceneComponent* m_pCameraBase;	// カメラの原点

	UPROPERTY(EditAnywhere, Category = "Camera")
		class UCameraComponent* m_pCamera;		// カメラ

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_eyeLevelWhenStanding;			// 立っているときの目の高さ

	UPROPERTY(EditAnywhere, Category = "Camera")
		float camera_shaking_value;				// 歩行時のカメラの揺れ具合

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputPitch;				// Pitch操作反転

	UPROPERTY(EditAnywhere, Category = "Camera")
		bool m_reverseInputYaw;					// Yaw操作反転

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraRotateSpeed;				// カメラの回転スピード

	UPROPERTY(EditAnywhere, Category = "Check")
		float m_CheckToActorRayRange;

	bool isStanding;							// 立っているかどうかのフラグ
	bool isVRCamera;							// VRカメラかどうか

	bool m_isStanding;							// 立っているかどうかのフラグ

	USoundBase* sound_player_footstep_;

	float count_for_footstep_;
	float eyelevel_for_camera_shaking;

	bool can_make_footstep;

	float m_playerMoveSpeed;					// プレイヤーの移動速度

	FVector2D m_playerMoveInput;				// プレイヤーの移動入力量
	FVector2D m_cameraRotateInput;				// カメラの回転入力量

	AItemBase* m_pCheckingItem;					// チェック中のアイテム
	AItemBase* m_pPrevCheckItem;				// 1フレーム前にチェックしていたアイテム

	// スマホオブジェクト(作成者：尾崎)
	UChildActorComponent* smartphone;

	bool isHaveSmartphoneFlag;		// スマホを構えるかどうか(作成者：尾崎)
	bool shatterFlag;				// シャッターを切るか(作成者：尾崎)
	bool lightFlag;					// ライトの切り替え(作成者：尾崎)


	// =====  VR Motion コントローラー　プロパティ  by_Rin =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRMotion|Variables", meta = (AllowPrivateAccess = "true"))
		class AThrillerVR_MotionController* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VRMotion|Variables", meta = (AllowPrivateAccess = "true"))
		class AThrillerVR_MotionController* RightController;

	FHitResult vr_HitResult;

	FString path = "Blueprint'/Game/Blueprints/BP_VRSmartPhone.BP_VRSmartPhone_C'";

	// VR's smart phone component
	UPROPERTY(EditAnywhere, Category = "VR_Phone")
		TSubclassOf<class AActor> bp_VRphone;

	UPROPERTY(EditAnywhere, Category = "VR_Phone")
		AActor* vr_Phone;

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		bool vr_InCameraMode;

	bool isFound;		// 敵の攻撃範囲内に入ったか(作成者：尾崎)

	UFUNCTION(BlueprintCallable, Category = "PlayerDamage")
		void SetIsFound(const bool _flag) { isFound = _flag; }	// 見つかった場合にisFoundを変える関数(作成者：尾崎)

	void Respawn();	// リスポーンする関数(作成者：尾崎)

	//UFUNCTION(BlueprintCallable, Category = "PlayerMakeNoise")
};