//-------------------------------------------------------------------
// �t�@�C��		�FGlobalLightController.h
// �쐬��		�F19CU0217 ��K
// �쐬��		�F2020/11/18
// �T�v			�F�S�Ẵ��C�g�𐧌䂷��R���g���[���[
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalLightController.generated.h"

// �O���錾
class ULightComponent;

UCLASS()
class HUKUOKAGAME_API AGlobalLightController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalLightController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// �S�Ẵ��C�g���i�[����z��
	TArray<ULightComponent*> lightComponentsArray;

public:
	// �S�Ẵ��C�g�̐F��ύX����
	UFUNCTION(BlueprintCallable, Category = "GlobalLightController")
	void SetLightsColor(FLinearColor _color);
};
