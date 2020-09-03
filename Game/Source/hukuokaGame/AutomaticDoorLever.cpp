//-------------------------------------------------------------------
// ファイル		：AutomaticDoorLever.cpp
// 概要			：自動ドアのロック、解除を管理する
// 作成者		：19CU0233 増井悠斗
// 作成日		：2020/08/24
//-------------------------------------------------------------------


#include "AutomaticDoorLever.h"

// Sets default values
AAutomaticDoorLever::AAutomaticDoorLever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

