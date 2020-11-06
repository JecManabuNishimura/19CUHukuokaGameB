//-------------------------------------------------------------------
// �ե�����		��HeartBeatAppWidgetComponent.h
// ��Ҫ			�����������ץ�Υ��饹
// ������		��19CU0217 ���m
// ������		��2020/11/4
// ������		��2020/11/6 isAppOn�ե饰��֥�`�ץ��Ȥ˥��������Ǥ���褦�ˤ���
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HeartBeatAppWidgetComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = "UserInterface", hidecategories = (Object, Activation, "Components|Activation", Sockets, Base, Lighting, LOD, Mesh), editinlinenew, meta = (BlueprintSpawnableComponent))
class HUKUOKAGAME_API UHeartBeatAppWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	// ���󥹥��󥹤�ȡ�ä����v��
	static UHeartBeatAppWidgetComponent* GetInstance() { return _instance; }
	// begin play
	virtual void BeginPlay() override;

public:
	// ���ץ������״�B���O�������v��
	void SetAppStat(bool _bool);

	// isAppOn��ȡ�ä����v���������ߣ����m��
	UFUNCTION(BlueprintGetter)
	bool GetIsAppOn() { return isAppOn; }

	// isAppOn���O�������v���������ߣ����m��
	UFUNCTION(BlueprintSetter)
	void SetIsAppOn(bool _bool) { isAppOn = _bool; }


	// isScaleing��ȡ�ä����v���������ߣ����m��
	UFUNCTION(BlueprintGetter)
	bool GetIsScaling() { return isScaling; }

	// isScaleing���O�������v���������ߣ����m��
	UFUNCTION(BlueprintSetter)
	void SetIsScaling(bool _bool) { isScaling = _bool; }

	// scaleSize��ȡ�ä����v���������ߣ����m��
	UFUNCTION(BlueprintGetter)
	float GetScaleSize() { return scaleSize; }

	// beatDelay��ȡ�ä����v���������ߣ����m��
	UFUNCTION(BlueprintGetter)
	float GetBeatDelay() { return beatDelay; }

	// beatDelay���O�����뤹���v���������ߣ����m��
	UFUNCTION(BlueprintSetter)
	void SetBeatDelay(float _value) { beatDelay = _value; }

private:
	// �֥�`�ץ��Ȥ�ȡ�äǤ���褦�ˤ��롡�������ߣ����m��
	// heart beat app is on/off
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsAppOn, BlueprintGetter = GetIsAppOn, Category = "AppWidget Parameters")
		bool isAppOn;
	// heart is Scaleing or not
	UPROPERTY(EditAnywhere, BlueprintSetter = SetIsScaling, BlueprintGetter = GetIsScaling, Category = "AppWidget Parameters")
		bool isScaling;
	// beat scale
	UPROPERTY(EditAnywhere, BlueprintGetter = GetScaleSize, Category = "AppWidget Parameters")
		float scaleSize;
	// beat delay
	UPROPERTY(EditAnywhere, BlueprintSetter = SetBeatDelay, BlueprintGetter = GetBeatDelay, Category = "AppWidget Parameters")
		float beatDelay;
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


};
