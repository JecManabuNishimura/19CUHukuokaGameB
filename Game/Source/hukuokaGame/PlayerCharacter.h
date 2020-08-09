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
	// カメラの更新
	void UpdateCamera(const float _deltaTime);

	// プレイヤーの移動処理
	void UpdatePlayerMove(const float _deltaTime);

	// ベクトルの長さを返す
	float ReturnVector2DLength(const FVector2D* _pFvector2d);

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

private:
	// プロパティ
	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerThresholdToRun;			// 走る閾値(0 < this <= 1)

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerRunSpeed;					// プレイヤーの歩く速度

	UPROPERTY(EditAnywhere, Category = "Move")
		float m_playerWalkSpeed;				// プレイヤーの走る速度

	float m_playerMoveSpeed;					// プレイヤーの移動速度

	FVector2D m_playerMoveInput;				// プレイヤーの移動入力量
	FVector2D m_cameraRotateInput;				// カメラの回転入力量
};
