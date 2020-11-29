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
// 更新日		：2020/11/04		心拍数アプリのアクションマッピングを追加
// 更新日		：2020/11/05		インスタンスがあるかどうかの確認を追加
// 更新日		：2020/11/16		ダメージ状態２を追加
// 更新日		：2020/11/17		ダメージ状態１、２の処理を追加
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
#include "Camera/CameraComponent.h"
#include "Sound/SoundClass.h"
#include "HeartBeatAppWidgetComponent.h"	// 心拍数アプリ用（作成者：朱適）
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

	// =====  VR Motion コントローラー 関数  (作成者:林雲暉) =====
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

	// =====  VR Motion コントローラー ポインターの関数  (作成者:林雲暉) =====
	void UpdateVRLaser();
	void CheckStandingVR();

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
	void PlayerStand();
	// プレイヤーアクション：しゃがむ
	void PlayerSquat();

	// プレイヤーアクション：拾う、調べる、作動させる
	void CheckToActor();

	// スマホ関係
	// 構えるかどうかのフラグ(作成者：尾崎)
	void ChangeHaveSmartphoneFlag();

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
	UPROPERTY(EditAnywhere, Category = "Smart Phone App")
	bool isHeartBeatOn;			// 心拍数アプリの切り替え(作成者：朱適)
	// 心拍数アプリの切り替え(作成者：朱適)
	// Private->Publicに変更(UPROPETY変数宣言をprivateにしない方が良い)
	void HeartBeatStatusSwitch()
	{
		// VRコントロールに対応するため追加判定(作成者:林雲暉)
		if (smartPhone_Mode_Num == 3)
		{
			isHeartBeatOn = !isHeartBeatOn;
			// インスタンスがあるかどうかの確認(作成者：朱適)
			if (UHeartBeatAppWidgetComponent::GetInstance())
			{
				UHeartBeatAppWidgetComponent::GetInstance()->SetAppStat(isHeartBeatOn);
			}
			UE_LOG(LogTemp, Log, TEXT("HeartBeat App Status Switched"));
		} // end if()

	}
	// Smartphoneから呼び出す関数(isHaveSmartphoneFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetisHaveSmartphoneFlag();

	// Smartphoneから呼び出す関数(lightFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetLightFlag();

	// Smartphoneから呼び出す関数(shutterFlagをSmartphoneに送る)(作成者：尾崎)
	bool GetShatterFlag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
		bool in_the_locker_;	// ロッカーに入ろうとする～完全に出るまでのフラグ(作成者：尾崎

	void SetInTheLocker(const bool flag);				// 定義がcppに移りました　(作成者:林雲暉)

	// Phoneアクターを取得する関数（作成者：朱適）
	UFUNCTION(BlueprintGetter)
		AActor* GetPhoneActor() { return vr_Phone; };

public:
	UFUNCTION(BlueprintCallable, Category = "Return State")
		AItemBase* ReturnCheckingItem() const;

	UFUNCTION(BlueprintCallable, Category = "Return State")
		FString ReturnCheckingItemCommandName() const;

	unsigned char player_state;

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
		UCameraComponent* m_pCamera;		// カメラ

	UPROPERTY(EditAnywhere, Category = "Camera")
		class USpringArmComponent* m_pSpringArm;		// スプリングアーム  (作成者:林雲暉)

	UPROPERTY(EditAnywhere)
		USoundClass* se_volume_can_change_;		// 各効果音に設定しているサウンドクラス

	UPROPERTY(EditAnywhere)
		USoundBase* sound_player_footstep_;		// プレイヤーの足音SE

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

	UPROPERTY(EditAnywhere, Category = "Trace")
		float check_to_actor_trace_length_;

	UPROPERTY(Editanywhere, Category = "Trace")
		bool draw_debug_trace_;

	UPROPERTY(Editanywhere, Category = "Trace")
		FVector box_half_size_;

	EDrawDebugTrace::Type draw_debug_trace_type_;

	// 使ってない
	bool isVRCamera;							// VRカメラかどうか

	float count_for_footstep_;					// 足音のためのカウント

	float eyelevel_for_camera_shaking;			// カメラの縦揺れの度合い

	bool can_make_footstep;						// 足音発生許可フラグ

	bool can_player_control;					// プレイヤーの操作が出来るか

	float m_playerMoveSpeed;					// プレイヤーの移動速度

	int damage_count_;							// 敵から攻撃を受けた回数(1回…聴覚デバフ、2回…触覚デバフ、3回…視覚デバフ、4回…死)

	FVector2D m_playerMoveInput;				// プレイヤーの移動入力量
	FVector2D m_cameraRotateInput;				// カメラの回転入力量

	AItemBase* m_pCheckingItem;					// チェック中のアイテム
	AItemBase* m_pPrevCheckItem;				// 1フレーム前にチェックしていたアイテム)

	//-----------------------------------------------------------------------------------------------------------------
	// 聴覚デバフのためのプロパティ各効果音の音量に適用して疑似的に聴覚が弱まったようにみせる(作成者 増井悠斗)
	//-----------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Hearing Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Sound Volume"))
		float se_volume_for_debuff_;

	//-----------------------------------------------------------------------------------------------------------------
	// 視覚デバフのためのポストプロセスプロパティ(作成者 増井悠斗)
	//-----------------------------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "2.0", DisplayName = "Color Saturation"))
		FVector4 saturation_for_debuff_;			// 彩度(グレースケールにするためにはWを0に)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "2.0", DisplayName = "Color Contrast"))
		FVector4 contrast_for_debuff_;				// コントラスト

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Vignette Intensity"))
		float vignette_intensity_for_debuff_;		// ビネット効果強度(画面周りが暗くなる)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Grain Intensity"))
		float grain_intensity_for_debuff_;			// グレイン強度(ノイズ)

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "Film Slope"))
		float film_slope_for_debuff_;

	UPROPERTY(EditAnywhere, Category = "Visual Debuff", meta = (UIMin = "0.0", UIMax = "1.0", DisplayName = "FilmToe"))
		float film_toe_for_debuff_;
	//-----------------------------------------------------------------------------------------------------------------

	// コンストラクタ無し
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

	// コンストラクタ無し
	UPROPERTY(EditAnywhere, BlueprintGetter = GetPhoneActor, Category = "VR_Phone", meta = (AllowPrivateAccess = "true"))
		AActor* vr_Phone;


public: 
	// ===== 移動としゃがむ　プロパティ  (作成者:林雲暉) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		bool m_isStanding;							// 立っているかどうかのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Walk ;					// 歩いての移動速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Run;					// 走るの移動速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NewMove")
		float m_MaxWalkSpeed_Crouch;				// しゃがむの移動速度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		float m_VRPlayersHeight;					// VRモードの時現実プレイヤーの立っているの高さ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		bool m_HeightisChecked;						// VRのプレイヤー身長確認しましたか

	// コンストラクタ無し
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
		bool vr_isVRmode;							// VRモードかどうかのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		bool vr_InCameraMode;						// VRのカメラモードかどうかのフラグ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		int holdingSmartphoneState;					// スマホをを手前に持っているか?

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		int vr_SmartPhone_Mission_Num;				// 今表示するミッションナンバー

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone")
		int smartPhone_Mode_Num;					// スマホアプリモード

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		FString vr_SmartPhone_Mission_Contents;		// 今表示するミッション　（今は使えていない）

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR_Phone|Mission")
		int finished_MsiionID;						// テスト用　　　完成したミッションID　（表示したくないIDを入れる）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStatus")
		TArray<int> missionStatus ;					// ミッションの完了状態を管理するarray

	// 0:Insert, 1:Delete, 2:Update
	UFUNCTION(BlueprintCallable, Category = "VR_Phone|Mission")
		void UpdateTheMission(int _updateMode, int _missionID, bool& _hasUpdated);	// _missionIDというミッションのフラグと表示を処理する

	// using for return bool. The calling  blueprint function can't return variable, so using players's variable to store the bool.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR_Phone|Mission")
		bool missionTableHasUpdated;											// スマホのミッションがアップデートされましたか(Blueprint用)

	UFUNCTION(BlueprintCallable, Category = "VR_Phone|Mission")
		int GetTheWideStringsByteLength(FString _inString, FText _inText);		// 文字数のByteをカウントする

	bool isFound;		// 敵の攻撃範囲内に入ったか(作成者：尾崎)

	UFUNCTION(BlueprintCallable, Category = "PlayerDamage")
		void SetIsFound(const bool _flag, FVector _enemy_location);			// 見つかった場合にisFoundを変える関数(作成者：尾崎)

	// ダメージを受ける時にBPから呼ばれる関数
	UFUNCTION(BlueprintCallable, Category = "PlayerDamage")
		void AttackFromEnemy();

	// ダメージ状態１になった時呼び出す関数。Blueprintにオーバーライドする（作成者：朱適）
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void EarDamaged();

	// ダメージ状態２になった時呼び出す関数。Blueprintにオーバーライドする（作成者：朱適）
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void HandDamaged();

	// ダメージ状態３になった時呼び出す関数。Blueprintにオーバーライドする（作成者：朱適）
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerDamage")
		void EyeDamaged();
	
	FVector ReturnCameraLocation();

	FVector ReturnCameraForwardVector();

	void SetPlayerControlFlag(bool _flag) { can_player_control = _flag; }

	void Respawn();	// リスポーンする関数(作成者：尾崎)

	//UFUNCTION(BlueprintCallable, Category = "PlayerMakeNoise")
};