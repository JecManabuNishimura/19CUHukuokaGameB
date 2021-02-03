// Fill out your copyright notice in the Description page of Project Settings.

#include "UObject/ConstructorHelpers.h"		// �R���X�g���N�^�Ŏg��
#include "EnemyMyAIController.h"


AEnemyMyAIController::AEnemyMyAIController(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // �u���b�N�{�[�h�E�r�w�C�r�A�c���[�̃R���|�[�l���g�쐬
    BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
    BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

    // �r�w�C�r�A�c���[���p�X�������ĕϐ��Ɋi�[
    ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/C++Enemy/Enemy_BT"));
    BehaviorTree = BTFinder.Object;

    PlayerActorKeyName = "PlayerActor";
}

void AEnemyMyAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyMyAIController::OnPossess(class APawn* InPawn)
{
    Super::OnPossess(InPawn);
    BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);

    BehaviorComp->StartTree(*BehaviorTree);
}


void AEnemyMyAIController::OnUnPossess()
{
    Super::OnUnPossess();
    BehaviorComp->StopTree();
}

// PawnSensing�̎���̒��ɓ��������Ăяo�����
void AEnemyMyAIController::SetPlayerActorKey(APawn* Goal)
{
    if (BlackboardComp != NULL)
    {
        BlackboardComp->SetValueAsObject(PlayerActorKeyName, Goal);
    }
}

APlayerCharacter* AEnemyMyAIController::GetPlayerActorKey()
{
    if (BlackboardComp != NULL)
    {
        return Cast<APlayerCharacter>(BlackboardComp->GetValueAsObject(PlayerActorKeyName));
    }
    return nullptr;
}

