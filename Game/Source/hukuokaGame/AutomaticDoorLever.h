//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.h
// �T�v			�F�����h�A�̃��b�N�A�������Ǘ�����
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AutomaticDoorLever.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorLever : public AItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutomaticDoorLever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable, Category = "ReturnState")
		bool ReturnLeverState(){ return m_isLeverOn; }

	UPROPERTY(EditAnywhere)
		int m_leverFilter;					// ���o�[�ƃh�A��Ή������邽�߂̐���

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;
};
