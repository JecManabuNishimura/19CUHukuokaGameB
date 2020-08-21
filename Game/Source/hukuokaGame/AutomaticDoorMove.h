//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorMove.h
// �T�v			�F�����h�A�𐧌䂷��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/20
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"				// UBoxComponent���g�����߂ɕK�v�ȃw�b�_
#include "AutomaticDoorMove.generated.h"

UCLASS()
class HUKUOKAGAME_API AAutomaticDoorMove : public AActor
{
	GENERATED_BODY()
	
public:
	// �R���X�g���N�^
	AAutomaticDoorMove();
	// �f�X�g���N�^
	~AAutomaticDoorMove();

protected:
	// ���s���Ɉ�x�Ă΂��
	virtual void BeginPlay() override;

public:	
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

private:


private:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEventTriggerBox;	// �v���C���[�A�G���m�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		float m_detectSpan;					// �G�A�v���C���[�̌��m�X�p��

	UPROPERTY(EditAnywhere)
		float m_doorStartPosY;				// �h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float m_doorEndPosY;				// �h�A��Y���W�I�_

	UPROPERTY(EditAnywhere)
		bool m_isLeverOn;					// �����h�A���쓮�����邽�߂̃��o�[�t���O

	float m_openTimeCount;					// �J���Ă��鎞�Ԃ̃J�E���g

};
