//-------------------------------------------------------------------
// ファイル		：Smartphone.h
// 概要			：スマートフォンの制御
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/09/15
//-------------------------------------------------------------------
#pragma once

class APlayerCharacter;

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PointLightComponent.h"
#include "Components/ChildActorComponent.h"

#include "kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "Smartphone.generated.h"

UCLASS()
class HUKUOKAGAME_API ASmartphone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASmartphone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// Playerを取得する
	void GetPlayer();
	// スマホの構え状態を変更する
	void ChangeHaveSmartphone();
	// ライトの状態を変更する
	void SetIsLight();

public:

public:
	// BP上で参照可能に
	UPROPERTY(BlueprintReadWrite, Category = "ChangeLight")
		bool isLight;

	APlayerCharacter* player;
};
