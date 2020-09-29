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
#include "Components/StaticMeshComponent.h"
#include "AutomaticDoorLever.h"
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
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex);
		
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEventTriggerBox;		// �v���C���[�A�G���m�p�g���K�[�{�b�N�X

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pLeftDoorComp;	// ���h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_pRightDoorComp;	// �E�h�A�̃��b�V���R���|�[�l���g

	UPROPERTY(EditAnywhere)
		float m_detectSpan;						// �G�A�v���C���[�̌��m�X�p��

	UPROPERTY(EditAnywhere)
		float m_openAndCloseTime;				// �J�ɗv���鎞��

	float m_leftDoorStartPosY;					// ���h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		float m_leftDoorEndPosY;				// ���h�A��Y���W�I�_

	float m_rightDoorStartPosY;					// �E�h�A��Y���W�n�_

	UPROPERTY(EditAnywhere)
		int m_doorFilter;						// ���o�[�ƃh�A��Ή������邽�߂̐���

	bool m_isSwitchOn;							// �����h�A���쓮�����邽�߂̃��o�[�t���O

	bool m_canMove;								// �h�A�̊J�������s�����̃t���O

	bool m_isOpened;							// �h�A���J�������Ă��邩�̃t���O

	bool m_isOpening;							// �h�A���J���r�����̃t���O

	bool m_isOverlap;							// �v���C���[�������͓G�����m���ɓ����Ă��邩�̃t���O

	float m_openTimeCount;						// �J���Ă��鎞�Ԃ̃J�E���g

	float m_leftDoorMoveDirection;				// �ړ�����Ƃ��̌���

	float m_distanceStartToEnd;					// �h�A�̎n�_����I�_�܂ł̋���(�Е��Ŏ擾���A�����Е��ł�����𗘗p)

	void CheckDetectSpan(float _deltaTime);		// �h�A���J����������̌��m�`�F�b�N

	void UpdateDoorMove(float _deltaTime);		// �h�A�̍X�V
};
