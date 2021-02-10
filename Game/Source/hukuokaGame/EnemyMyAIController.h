// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"				// �r�w�C�r�A�c���[
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"		// �u���b�N�{�[�h
#include "EnemyMyAIController.generated.h"

UCLASS()
class HUKUOKAGAME_API AEnemyMyAIController : public AAIController
{
    GENERATED_BODY()

        // �R���X�g���N�^
        AEnemyMyAIController();//(const class FObjectInitializer& ObjectInitializer);
    
    // �r�w�C�r�A�c���|�i�[�p
    //UBehaviorTreeComponent* BehaviorComp;

    // �u���b�N�{�[�h�i�[�p
    //UBlackboardComponent* BlackboardComp;

    /* Called whenever the controller possesses a character bot */
    virtual void OnPossess(class APawn* InPawn) override;

    virtual void OnUnPossess() override;

    UPROPERTY(EditDefaultsOnly, Category = AI)
        FName PlayerActorKeyName;

public:

    void SetPlayerActorKey(APawn* Goal);

    APlayerCharacter* GetPlayerActorKey();

protected:
    virtual void BeginPlay() override;

    //UPROPERTY(EditDefaultsOnly, Category = AI)
        //class UBehaviorTree* BehaviorTree;

    //FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

    //FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
