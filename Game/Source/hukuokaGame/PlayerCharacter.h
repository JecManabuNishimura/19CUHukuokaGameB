//-------------------------------------------------------------------
// ファイル		：PlayerCharacter.h
// 概要			：プレイヤーキャラを制御する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/07
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

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
	
private:
	// 地面のとの距離を測りプレイヤーの高さを設定
	void SetEyeLevel(const float _deltaTime);

	// カメラ(Pitch)の更新
	void UpdateCameraPitch();

	// カメラ(Yaw)の更新
	void UpdateCameraYaw();

	// プレイヤーの移動処理
	void UpdatePlayerMove(const float _deltaTime);

	// ベクトルの長さを返す
	float ReturnVector2DLength(const FVector2D* _pFvector2d);

	// ベクトルを正規化する
	void NormalizedVector2D(float _vectorLength, FVector2D* _pFvector2d);

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
	void PlayerStand() { isStanding = true; };
	// プレイヤーアクション：しゃがむ
	void PlayerSquat() { isStanding = false; };

private:
	// プロパティ
	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerThresholdToRun;			// 走る閾値(0 < this <= 1)

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerRunSpeed;					// プレイヤーの歩く速度

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerWalkSpeed;				// プレイヤーの走る速度

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMin;			// カメラのPitch制限最小角度

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_cameraPitchLimitMax;			// カメラのPitch制限最大角度

	UPROPERTY(EditAnywhere, Category = "Camera")
	class UCameraComponent* m_pCamera;			// カメラ

	UPROPERTY(EditAnywhere, Category = "Camera")
		float m_eyeLevelWhenStanding;			// 立っているときの目の高さ

	bool isStanding;							// 立っているかどうかのフラグ

	float m_playerMoveSpeed;					// プレイヤーの移動速度

	FVector2D m_playerMoveInput;				// プレイヤーの移動入力量
	FVector2D m_cameraRotateInput;				// カメラの回転入力量
};