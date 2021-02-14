//-------------------------------------------------------------------
// ファイル		：GlobalLightController.cpp
// 作成者		：19CU0217 朱適
// 作成日		：2020/11/18
// 概要			：全体的なライトを制御するコントローラー
// 更新履歴		：2020/12/22		ライトのMeshComponentの色変えを追加しました
// 更新履歴		：2021/02/13		複数のライトコンポネントに対応しました
//-------------------------------------------------------------------

#include "GlobalLightController.h"
#include "Components/LightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/StaticMeshComponent.h"
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
		// LightComponentを取得する
		// auto lightCmp = actor->GetComponentByClass(ULightComponent::StaticClass());
		// 複数のライトに対応しました		by 朱適
		auto lightCmpList = actor->GetComponentsByClass(ULightComponent::StaticClass());
		if (lightCmpList.Num() > 0)
		{
			for(auto* lightCmp : lightCmpList)
			{
				lightComponentsArray.Add(Cast<ULightComponent>(lightCmp));
			}
		}
		

		// MeshComponentを取得する		by 朱適
		auto lightMeshCmpList = actor->GetComponentsByClass(UStaticMeshComponent::StaticClass());
		if (lightMeshCmpList.Num() > 0)
		{
			for (auto* cmp : lightMeshCmpList)
			{
				if (cmp->ComponentHasTag(TEXT("LightMeshComponent")))
				{
					lightMeshCompArray.Add(Cast<UStaticMeshComponent>(cmp));
				}
			}
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
	// MeshComponentの色変え		by	朱適
	for (UStaticMeshComponent* meshCmp : lightMeshCompArray)
	{
		meshCmp->SetVectorParameterValueOnMaterials(TEXT("LightColor"), FVector::ForwardVector);
	}
}

