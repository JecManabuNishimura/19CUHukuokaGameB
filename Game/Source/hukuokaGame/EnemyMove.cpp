// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMove.h"

// Sets default values
AEnemyMove::AEnemyMove()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyMove::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyMove::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyMove::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

