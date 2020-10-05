//-------------------------------------------------------------------
// �t�@�C��		�FSmartphone.cpp
// �T�v			�F�X�}�[�g�t�H���̐���
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/09/15
//-------------------------------------------------------------------

#include "Smartphone.h"
#include "PlayerCharacter.h"

ASmartphone::ASmartphone()
	: isLight(false)
	, player(NULL)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �f�t�H���g�v���C���[�Ƃ��Đݒ�
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ASmartphone::BeginPlay()
{
	Super::BeginPlay();

	// Player���擾����֐�
	GetPlayer();
}

// Called every frame
void ASmartphone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// �V���b�^�[�@�\���Ȃ����ߕۗ�
	//if (player->GetShatterFlag())
	//{

	//}

	// �X�}�z�̍\����Ԃ�ύX
	ChangeHaveSmartphone();
	// ���C�g�̏�Ԃ̕ύX
	SetIsLight();
}

// Called to bind functionality to input
void ASmartphone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Player���擾����֐�
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

// �\����Ԃ�ύX����֐�
void ASmartphone::ChangeHaveSmartphone()
{
	// �t���O���̂�Player���ێ�
	this->SetActorHiddenInGame(player->GetisHaveSmartphoneFlag());
}

// ���C�g��ύX����t���O
void ASmartphone::SetIsLight()
{
	// �t���O���̂�Player���ێ�
	isLight = player->GetLightFlag();
}