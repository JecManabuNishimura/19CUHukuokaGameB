//-------------------------------------------------------------------
// �ե�����		��HeartBeatAppWidgetComponent.cpp
// ��Ҫ			�����������ץ�Υ��饹
// ������		��19CU0217 ���m
// ������		��2020/11/4
//-------------------------------------------------------------------

#include "HeartBeatAppWidgetComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstance.h"

// ���󥹥��󥹤���ڻ�
UHeartBeatAppWidgetComponent* UHeartBeatAppWidgetComponent::_instance = NULL;
// ���󥹥ȥ饯��

void UHeartBeatAppWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	_instance = this;
}

// �ʶȤ��O�������v��
void UHeartBeatAppWidgetComponent::_setColorScale(const float _scale)
{
	matInst->ScalarParameterValues[0].ParameterValue = _scale;
	//this->RequestRedraw();
	//Cast<UMeshComponent>(this)->SetScalarParameterValueOnMaterials(TEXT("GrayScale"), _scale);
}

// ���ץ������״�B���O�������v��
void UHeartBeatAppWidgetComponent::SetAppStat(bool _bool)
{
	isAppOn = _bool;
	if (isAppOn)
	{
		_setColorScale(grayScale);
		UE_LOG(LogTemp, Log, TEXT("set heart beat off"));
	}
	else
	{
		_setColorScale(colorfulScale);
		UE_LOG(LogTemp, Log, TEXT("set heart beat on"));
	}
}
