//-------------------------------------------------------------------
// ファイル		：Smartphone.cpp
// 概要			：スマートフォンの制御
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/09/15
//-------------------------------------------------------------------

#include "Smartphone.h"
#include "PlayerCharacter.h"

ASmartphone::ASmartphone()
	: isLight(false)
	, player(NULL)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// デフォルトプレイヤーとして設定
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ASmartphone::BeginPlay()
{
	Super::BeginPlay();

	// Playerを取得する関数
	GetPlayer();
}

// Called every frame
void ASmartphone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// シャッター機能がないため保留
	//if (player->GetShatterFlag())
	//{

	//}

	// スマホの構え状態を変更
	ChangeHaveSmartphone();
	// ライトの状態の変更
	SetIsLight();
}

// Called to bind functionality to input
void ASmartphone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Playerを取得する関数
void ASmartphone::GetPlayer()
{
	TSubclassOf<APlayerCharacter> findClass = APlayerCharacter::StaticClass();
	TArray<AActor*> pPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, pPlayers);
	if (pPlayers.Num())
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(pPlayers[0]);
		FString message = FString("PlayerName") + pPlayer->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, message);

		player = pPlayer;
	}
}

// 構え状態を変更する関数
void ASmartphone::ChangeHaveSmartphone()
{
	// フラグ自体はPlayerが保持
	this->SetActorHiddenInGame(player->GetisHaveSmartphoneFlag());
}

// ライトを変更するフラグ
void ASmartphone::SetIsLight()
{
	// フラグ自体はPlayerが保持
	isLight = player->GetLightFlag();
}