//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.h
// 概要			：自動ドアのロック、解除を管理する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorLever : public AItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutomaticDoorLever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable, Category = "ReturnState")
		bool ReturnLeverState(){ return m_isLeverOn; }

	UPROPERTY(EditAnywhere)
		int m_leverFilter;					// レバーとドアを対応させるための数字

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;
};
