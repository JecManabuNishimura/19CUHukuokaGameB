//-------------------------------------------------------------------
// �ե�����		��HeartBeatAppWidgetComponent.h
// ��Ҫ			�����������ץ�Υ��饹
// ������		��19CU0217 ���m
// ������		��2020/11/4
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HeartBeatAppWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class HUKUOKAGAME_API UHeartBeatAppWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	// ���󥹥��󥹤�ȡ�ä����v��
	static UHeartBeatAppWidgetComponent* GetInstance() { return _instance; }
	// begin play
	virtual void BeginPlay() override;

private:
	// heart beat app is on/off
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		bool isAppOn;
	// heart beat speed
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float beatSpeed;
	// beat scale
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float scaleSize;
	// beat frequency
	UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		float beatFrequency;
	//UPROPERTY(EditAnywhere, Category = "AppWidget Parameters")
		UMaterialInstance* matInst;

	// ɫ�Υ����`��
	// ���Ӥ��Ƥ��ʤ��r
	const float grayScale = 0.5f;
	// ���Ӥ��Ƥ���r
	const float colorfulScale = 1.0f;

	// ���󥹥���
	static UHeartBeatAppWidgetComponent* _instance;

private:
	// �ʶȤ��O�������v��
	void _setColorScale(const float _scale);
public:
	// ���ץ������״�B���O�������v��
	void SetAppStat(bool _bool);

};
