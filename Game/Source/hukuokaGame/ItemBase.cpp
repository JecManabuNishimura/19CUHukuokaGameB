//-------------------------------------------------------------------
// �t�@�C��		�FItemBase.cpp
// �T�v			�F�E����A���ׂ���A�쓮�ł���Actor�̊�{�N���X�A
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/11
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.h
// �쐬��		�F19CU0217 ��K
// �X�V��		�F2020/10/14		���g�̕\���̒ǉ�
//-------------------------------------------------------------------

#include "ItemBase.h"
// PrimitiveComponent���C���N���[�h		by	��K
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
AItemBase::AItemBase()
	: sound_when_checked_(NULL)
	, m_isChecked(false)
	, m_commandName("")
	, items_Mission_Num(0)
	, isMissionComplete(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ���g�̕\���̐���	by ��K (12/13 �C����:���� �����ɃR���|�[�l���g�̃C���f�b�N�X��ǉ�)1
void AItemBase::SetOutline(bool _isCustomDepthOn, const int _checking_comp_index)
{
	//UActorComponent* meshComponent = GetComponentByClass(UStaticMeshComponent::StaticClass());

	USceneComponent* sceneComponent = GetRootComponent()->GetChildComponent(_checking_comp_index);

	if (sceneComponent)
	{
		Cast<UPrimitiveComponent>(sceneComponent)->SetRenderCustomDepth(_isCustomDepthOn);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("cant find outline mesh"));
	}

}
