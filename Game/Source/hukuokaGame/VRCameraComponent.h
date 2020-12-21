//--------------------------------------------------------------------------
// �t�@�C��		�FVRCameraComponent.h
// �T�v			�FVR camera override, Slow down the HMD camera move speed
// �쐬��		�F19CU0236 �щ_�� 
// �쐬��		�F2020/12/20
//--------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"
#include "Rendering/MotionVectorSimulation.h"
#include "IXRTrackingSystem.h"
#include "IXRCamera.h"
#include "VRCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class HUKUOKAGAME_API UVRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR")
        float numToDevideSpeed;            // the number to devide of Slowing down the HMD camera move speed

public:
    UVRCameraComponent();

    virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

};
