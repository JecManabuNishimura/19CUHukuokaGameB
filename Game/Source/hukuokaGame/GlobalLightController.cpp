//-------------------------------------------------------------------
// �t�@�C��		�FGlobalLightController.cpp
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/11/18
// �T�v			�F�S�̓I�ȃ��C�g�𐧌䂷��R���g���[���[
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
	
	// �S�Ẵ��C�g���擾����
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

// �S�Ẵ��C�g�̐F��ύX����
void AGlobalLightController::SetLightsColor(FLinearColor _color)
{
	for (ULightComponent* light : lightComponentsArray)
	{
		light->SetLightColor(_color);
	}
}

