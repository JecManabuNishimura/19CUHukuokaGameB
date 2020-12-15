//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.cpp
// �T�v			�FItemBase�N���X���p�����鎩���h�A�쓮�̃��b�N�A�������Ǘ����郌�o�[
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------


#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

AAutomaticDoorLever::AAutomaticDoorLever()
	: m_leverFilter(-1)
	, m_isLeverOn(false)
	, can_control_(true)
	, m_pDoorBody(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();

	// �t�B���^�[�ԍ�����v����h�A�擾�A�ۑ�
	TSubclassOf<AAutomaticDoorBody> findClass;
	findClass = AAutomaticDoorBody::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorBody* pDoorBody = Cast<AAutomaticDoorBody>(actors[idx]);
			// �h�A�{�̂̃t�B���^�[�ԍ������o�[�̃t�B���^�[�ԍ��ƈ�v���Ă���Ίi�[����
			if (pDoorBody->GetDoorFilter() == m_leverFilter)
			{
				m_pDoorBody = pDoorBody;
				break;
			}
		}
		// ���o�[�ɑΉ�����h�A�����x���ɔz�u����Ă��Ȃ����߃G���[
		if (!m_pDoorBody)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	// ���������h�A��������x���ɔz�u����Ă��Ȃ����߃G���[
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no door installed on the level !"));
	}
}

void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AAutomaticDoorLever::CheckedByPlayer()
{
	// ���o�[�̑��삪�s�Ȃ�return
	if (!can_control_) return;

	// OFF��������ON��
	if (!m_isLeverOn)
	{
		// ���쉹��炷
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

		// ���o�[�̏�Ԃ𔽓]
		m_isLeverOn = true;

		// �h�A�{�̂̍쓮�t���O���X�V
		if (m_pDoorBody != NULL)
		{
			m_pDoorBody->UpdateSwitchState(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
}
