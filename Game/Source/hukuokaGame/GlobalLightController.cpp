//-------------------------------------------------------------------
// �t�@�C��		�FGlobalLightController.cpp
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/11/18
// �T�v			�F�S�̓I�ȃ��C�g�𐧌䂷��R���g���[���[
// �X�V����		�F2020/12/22		���C�g��MeshComponent�̐F�ς���ǉ����܂���
// �X�V����		�F2021/02/13		�����̃��C�g�R���|�l���g�ɑΉ����܂���
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
		// LightComponent���擾����
		// auto lightCmp = actor->GetComponentByClass(ULightComponent::StaticClass());
		// �����̃��C�g�ɑΉ����܂���		by ��K
		auto lightCmpList = actor->GetComponentsByClass(ULightComponent::StaticClass());
		if (lightCmpList.Num() > 0)
		{
			for(auto* lightCmp : lightCmpList)
			{
				lightComponentsArray.Add(Cast<ULightComponent>(lightCmp));
			}
		}
		

		// MeshComponent���擾����		by ��K
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

// �S�Ẵ��C�g�̐F��ύX����
void AGlobalLightController::SetLightsColor(FLinearColor _color)
{
	for (ULightComponent* light : lightComponentsArray)
	{
		light->SetLightColor(_color);
	}
	// MeshComponent�̐F�ς�		by	��K
	for (UStaticMeshComponent* meshCmp : lightMeshCompArray)
	{
		meshCmp->SetVectorParameterValueOnMaterials(TEXT("LightColor"), FVector::ForwardVector);
	}
}

