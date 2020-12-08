//-------------------------------------------------------------------
// �t�@�C��		�FDestroyDoor.h
// �T�v			�F�Ԃ���񂪋������Ƃ��A�G���˂��j��h�A�̏����̍쐬
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/12/
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DestroyDoor.generated.h"

UCLASS()
class HUKUOKAGAME_API ADestroyDoor : public AActor
{
	GENERATED_BODY()

public:
	// �R���X�g���N�^
	ADestroyDoor();

protected:
	// �Q�[���J�n or ������
	virtual void BeginPlay() override;

public:
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

private:
	//bool m_is_baby_cry;

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pdoor_mesh_;		// ���{�̂̃��b�V��
	//UPROPERTY(EditAnywhere, Category = "Mesh")
		//UStaticMeshComponent* m_pdoorglass_mesh;	// ���ɕt���Ă���K���X�̃��b�V��(�]�͂�����Γ|���Ƃ��Ɋ��邱�Ƃ��o����悤�ɂ��邽��)
	UPROPERTY(EditAnywhere, Category = "Baby")
		bool m_is_baby_cry;								// �I�������private�Ɉړ�
	UPROPERTY(EditAnywhere, Category = "ImpulseValue")
		FVector m_Impulsevalue;
	UFUNCTION(BlueprintCallable, Category = "Baby")
		void GetIsCry(bool flag) { m_is_baby_cry = flag; }
};
