//-------------------------------------------------------------------
// ファイル		：GlobalLightController.cpp
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/18
// 概要			：全体的なライトを制御するコントローラー
//-------------------------------------------------------------------

#include "GlobalLightController.h"
#include "Components/LightComponent.h"
#include "Components/RectLightComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGlobalLightController::AGlobalLightController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGlobalLightController::BeginPlay()
{
	Super::BeginPlay();
	
	// 全てのライトを取得する
	UWorld* world;
	TArray<AActor*> lightActors;

	if (GetWorld() != NULL)
	{
		world = GetWorld();
		UGameplayStatics::GetAllActorsWithTag(Cast<UObject>(world), TEXT("Light"), lightActors);
	}

	for (AActor* actor : lightActors)
	{
		auto lightCmp = actor->GetComponentByClass(ULightComponent::StaticClass());
		if (lightCmp != NULL)
		{
			lightComponentsArray.Add(Cast<ULightComponent>(lightCmp));
		}
	}
}

// Called every frame
void AGlobalLightController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// 全てのライトの色を変更する
void AGlobalLightController::SetLightsColor(FLinearColor _color)
{
	for (ULightComponent* light : lightComponentsArray)
	{
		light->SetLightColor(_color);
	}
}

