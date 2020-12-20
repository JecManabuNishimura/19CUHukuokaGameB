//--------------------------------------------------------------------------
// ファイル		：VRCameraComponent.h
// 概要			：VR camera override, Slow down the HMD camera move speed
// 作成者		：19CU0236 林雲暉 
// 作成日		：2020/12/20
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
