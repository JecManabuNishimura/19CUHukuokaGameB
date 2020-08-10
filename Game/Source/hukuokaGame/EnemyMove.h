// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyMove.generated.h"


UCLASS()
class HUKUOKAGAME_API AEnemyMove : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyMove();
	// エディタ上に表示させるためのコマンド
	UPROPERTY(EditAnywhere)
	TArray<ATargetPoint*> TargetPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
