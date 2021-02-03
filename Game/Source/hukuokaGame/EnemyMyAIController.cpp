// Fill out your copyright notice in the Description page of Project Settings.

#include "UObject/ConstructorHelpers.h"		// コンストラクタで使う
#include "EnemyMyAIController.h"


AEnemyMyAIController::AEnemyMyAIController(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // ブラックボード・ビヘイビアツリーのコンポーネント作成
    BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
    BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

    // ビヘイビアツリーをパス検索して変数に格納
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

// PawnSensingの視野の中に入った時呼び出される
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

