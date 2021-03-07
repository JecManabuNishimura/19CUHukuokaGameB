//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.cpp
// �T�v			�FItemBase�N���X���p������E�����Ƃ��o����A�C�e��
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/09/26
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FItemCanPickup.cpp
// �쐬��		�F19CU0236 �щ_�� 
// �X�V��		�F2021/03/07			�A�C�e���q���g��ǉ�
//-------------------------------------------------------------------

#include "ItemCanPickup.h"

AItemCanPickup::AItemCanPickup()
	: player_character_(NULL)
	, cardkey_filter_(0)
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItemCanPickup::BeginPlay()
{
	Super::BeginPlay();
	
	player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// �A�C�e���q���g�𐶐����� (�쐬��:�щ_��)
	bp_ItemInfo = TSoftClassPtr<AActor>(FSoftObjectPath("Blueprint'/Game/Blueprints/BP_ItemHint3D.BP_ItemHint3D_C'")).LoadSynchronous();	// path�ɂ���N���X���擾
	if (bp_ItemInfo != nullptr && isNeedToDiaplsy == true)
	{
		itemInfoActor = GetWorld()->SpawnActor<AActor>(bp_ItemInfo);						// �A�C�e���q���g��Actor�Ƃ��Đ�������

		if (itemInfoActor != NULL)
		{

			itemInfoActor->SetActorEnableCollision(false);

			itemInfoActor->SetActorLocation((this->GetActorLocation() + infoPosition));

			FOutputDeviceNull ar;
			FString FuncName_and_Solution1 = FString::Printf(TEXT("InitialHeight "));
			FString FuncName_and_Solution2 = FString::Printf(TEXT("InitialScale "));
			FString FuncName_and_Solution3 = FString::Printf(TEXT("InitialDistance "));

			FuncName_and_Solution1 += FString::SanitizeFloat(infoWorkingHeight);
			FuncName_and_Solution2 += FString::SanitizeFloat(infoScale);
			FuncName_and_Solution3 += FString::SanitizeFloat(toPlayers_MinDistance);

			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution1, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution2, ar, NULL, true);
			itemInfoActor->CallFunctionByNameWithArguments(*FuncName_and_Solution3, ar, NULL, true);

		} // end if()
		else {
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist"));
	} // end else
}

void AItemCanPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AItemCanPickup::CheckedByPlayer()
{
	// �Ή�����t���O�𗧂Ă�
	player_character_->SetHaveCardkeyState(cardkey_filter_);

	// �擾����炷
	if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySound2D(GetWorld(), sound_when_checked_);

	// �~�b�V�����ɔ��f����ꍇ�A�X�}�z�̃~�b�V�������A�b�v�f�[�g  (�쐬��:�щ_��)
	if (this->isMissionComplete == false) {

		if (this->items_Mission_Num != 0)
		{
			player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
		} // end if()

	} // end if()

	// �E������A�C�e���q���g�������@(�쐬��:�щ_��)
	itemInfoActor->Destroy();

	// ���x���ォ�玩�g������
	this->Destroy();
}