//-------------------------------------------------------------------
// �t�@�C��		�FSmartphone.h
// �T�v			�F�X�}�[�g�t�H���̐���
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/09/15
//-------------------------------------------------------------------
#pragma once

class APlayerCharacter;

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PointLightComponent.h"
#include "Components/ChildActorComponent.h"

#include "kismet/GameplayStatics.h"
#include "Engine/Engine.h"

#include "Smartphone.generated.h"

UCLASS()
class HUKUOKAGAME_API ASmartphone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASmartphone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	// Player���擾����
	void GetPlayer();
	// �X�}�z�̍\����Ԃ�ύX����
	void ChangeHaveSmartphone();
	// ���C�g�̏�Ԃ�ύX����
	void SetIsLight();

public:

public:
	// BP��ŎQ�Ɖ\��
	UPROPERTY(BlueprintReadWrite, Category = "ChangeLight")
		bool isLight;

	APlayerCharacter* player;
};
