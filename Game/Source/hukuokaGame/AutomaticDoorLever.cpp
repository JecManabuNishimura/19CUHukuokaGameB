//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.cpp
// �T�v			�FItemBase�N���X���p�����鎩���h�A�쓮�̃��b�N�A�������Ǘ����郌�o�[
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/08/24
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FAutomaticDoorLever.cpp
// �쐬��		�F19CU0236 �щ_�� 
// �X�V��		�F2021/03/07			�A�C�e���q���g��ǉ�
//-------------------------------------------------------------------

#include "AutomaticDoorLever.h"
#include "AutomaticDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

AAutomaticDoorLever::AAutomaticDoorLever()
	: lever_filter_num_(-1) 
	, p_sound_when_lever_up_(NULL)
	, p_door_body_(NULL)
	, is_lever_on_(false)
	, can_control_(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAutomaticDoorLever::BeginPlay()
{
	Super::BeginPlay();

	// �t�B���^�[�ԍ�����v����h�A�擾�A�ۑ�
	TSubclassOf<AAutomaticDoorBody> findClass;
	findClass = AAutomaticDoorBody::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorBody* p_door_body = Cast<AAutomaticDoorBody>(actors[idx]);
			// �h�A�{�̂̃t�B���^�[�ԍ������o�[�̃t�B���^�[�ԍ��ƈ�v���Ă���Ίi�[����
			if (p_door_body->GetDoorFilter() == lever_filter_num_)
			{
				p_door_body_ = p_door_body;
				break;
			}
		}
		// ���o�[�ɑΉ�����h�A�����x���ɔz�u����Ă��Ȃ����߃G���[
		if (!p_door_body_)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	// ���������h�A��������x���ɔz�u����Ă��Ȃ����߃G���[
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("There is no door installed on the level !"));
	}

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
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid (Automatic Door Lever)"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist (Automatic Door Lever)"));
	} // end else

}

void AAutomaticDoorLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮�F��Ԕ��])
void AAutomaticDoorLever::CheckedByPlayer()
{
	// ���o�[�̑��삪�s�Ȃ�return
	if (!can_control_) return;

	// OFF��������ON��
	if (!is_lever_on_)
	{
		// ���쉹��炷(������)
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_when_checked_, GetActorLocation());

		// ���o�[�̏�Ԃ𔽓]
		is_lever_on_ = true;

		// �~�b�V�����ɔ��f����ꍇ�A�X�}�z�̃~�b�V�������A�b�v�f�[�g
		if (this->isMissionComplete == false) {

			if (this->items_Mission_Num != 0)
			{
				APlayerCharacter* player_character_;
				player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

				if (player_character_ != NULL)
				{
					player_character_->UpdateTheMission(2, this->items_Mission_Num, this->isMissionComplete);
				} // end if()
			} // end if()
		} // end if()


		// �h�A�{�̂̍쓮�t���O���X�V
		if (p_door_body_ != NULL)
		{
			p_door_body_->UpdateSwitchState(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("The corresponding door is not installed on the level !"));
		}
	}
	else
	{
		// ���쉹��炷(�グ��)
		if (p_sound_when_lever_up_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_when_lever_up_, GetActorLocation());
	}
}
