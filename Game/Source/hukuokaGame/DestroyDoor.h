//-------------------------------------------------------------------
// �t�@�C��		�FDestroyDoor.h
// �T�v			�F�Ԃ���񂪋������Ƃ��A�G���˂��j��h�A�̏����̍쐬
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/12/10
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
	float m_time_cnt_;	// �^�C���J�E���g
	bool m_impuls_flag_;
	bool m_is_baby_cry;

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UStaticMeshComponent* m_pdoor_mesh_;		// ���{�̂̃��b�V��
	//UPROPERTY(EditAnywhere, Category = "Baby")
	//	bool m_is_baby_cry;								// �I�������private�Ɉړ�
	UPROPERTY(EditAnywhere, Category = "ImpulseValue")
		FVector m_impulsevalue_;
	UFUNCTION(BlueprintCallable, Category = "Baby")
		void GetIsCry(bool flag) { m_is_baby_cry = flag; }
	UPROPERTY(EditAnywhere, Category = "Mesh")
		float m_disappea_collision_time_;		// ���b�ԓ����蔻����Ȃ�����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* glass_se_;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundBase* door_se_;
	UPROPERTY(EditAnywhere, Category = "Mesh")
		USceneComponent* root;
};
