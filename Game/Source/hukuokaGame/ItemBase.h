//-------------------------------------------------------------------
// ファイル		：ItemBase.h
// 概要			：拾える、調べられる、作動できるActorの基本クラス、
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/09/11
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class HUKUOKAGAME_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UFUNCTION(BlueprintCallable, Category = "Return State")
		bool ReturnCheckedState() { return m_isChecked; }

public:
	bool m_isChecked;			// プレイヤーにチェックされているか
};
