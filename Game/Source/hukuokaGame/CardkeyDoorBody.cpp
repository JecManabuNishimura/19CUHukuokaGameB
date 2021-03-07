//-------------------------------------------------------------------
// �t�@�C��		�FCardkeyDoorBody.cpp
// �T�v			�F�J�[�h�L�[�Ή��̃h�A�𐧌䂷��N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/22
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// �t�@�C��		�FCardkeyDoorBody.cpp
// �쐬��		�F19CU0236 �щ_�� 
// �X�V��		�F2021/03/07			�A�C�e���q���g��ǉ�
//-------------------------------------------------------------------

#include "CardkeyDoorBody.h"
#include "Engine.h"				// GEngine���Ăяo�����߂̃w�b�_

ACardkeyDoorBody::ACardkeyDoorBody()
	: p_door_body_eventtriggerbox_(NULL)
	, p_cardreader_mesh_1_(NULL)
	, p_cardreader_mesh_2_(NULL)
	, p_leftdoor_mesh_(NULL)
	, p_rightdoor_mesh_(NULL)
	, p_door_state_mesh_1_(NULL)
	, p_door_state_mesh_2_(NULL)
	, p_door_state_material_(NULL)
	, p_material_instance_dynamic_(NULL)
	, p_sound_door_open_(NULL)
	, p_sound_door_close_(NULL)
	, p_sound_loading_success_(NULL)
	, p_sound_loading_error_(NULL)
	, p_player_character_(NULL)
	, detect_span_(3.0f)
	, open_and_close_time_(1.0f)
	, check_cardkey_time_(2.0f)
	, display_error_time_(3.0f)
	, count_for_check_display_time_(0.0f)
	, left_door_start_posY_(0.0f)
	, left_door_end_posY_(0.0f)
	, right_door_start_posY_(0.0f)
	, door_filter_num_(0)
	, door_state_(kDoorStateClosed)
	, p_alert_lever_(NULL)
	, is_alert_lever_on_(false)
	, is_doorbody_eventbox_overlap_(false)
	, door_eventbox_overlap_sum_(0)
	, open_time_count_(0.0f)
	, required_time_for_open_and_close_(0.0f)
	, left_door_dire_for_move_(1.0f)
	, distance_start_to_end_(0.0f)
	, infoPosition2(FVector(0.f, 0.f, 20.f))
	, itemInfoActor2(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

	// Root�pSceneComponent����
	USceneComponent* scene_comp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	// �h�A�{�̂̌��m�g���K�[�{�b�N�X����
	p_door_body_eventtriggerbox_ = CreateDefaultSubobject<UBoxComponent>(TEXT("p_door_body_eventtriggerbox_"));

	// �J�[�h���[�_�[�̃��b�V������1�쐬
	p_cardreader_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_1"));

	// �J�[�h���[�_�[�̃��b�V������2�쐬
	p_cardreader_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_2"));

	// ���h�A�̃��b�V���쐬
	p_leftdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorMesh"));

	// �E�h�A�̃��b�V���쐬
	p_rightdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardReader_Mesh_Parent"));

	// ���h�A�̃��b�N��ԃ��b�V���쐬
	p_door_state_mesh_1_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoorStateMesh"));

	// �E�h�A�̃��b�N��ԃ��b�V���쐬
	p_door_state_mesh_2_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorStateMesh"));

	p_door_state_material_ = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/Materials/CardReaderStateMaterial")));
	
	if (scene_comp != NULL)				RootComponent = scene_comp;
	if (p_leftdoor_mesh_ != NULL)			p_leftdoor_mesh_->SetupAttachment(RootComponent);
	if (p_rightdoor_mesh_ != NULL)		p_rightdoor_mesh_->SetupAttachment(RootComponent);

	if (p_cardreader_mesh_1_ != NULL)		p_cardreader_mesh_1_->SetupAttachment(RootComponent);

	if (p_cardreader_mesh_2_ != NULL)		p_cardreader_mesh_2_->SetupAttachment(RootComponent);

	if (p_door_state_mesh_1_ != NULL)		p_door_state_mesh_1_->SetupAttachment(p_cardreader_mesh_1_);

	if (p_door_state_mesh_2_ != NULL)		p_door_state_mesh_2_->SetupAttachment(p_cardreader_mesh_2_);

	if (p_door_body_eventtriggerbox_ != NULL)
	{
		p_door_body_eventtriggerbox_->SetupAttachment(RootComponent);

		p_door_body_eventtriggerbox_->OnComponentBeginOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapBegin);
		p_door_body_eventtriggerbox_->OnComponentEndOverlap.AddDynamic(this, &ACardkeyDoorBody::OnDoorBodyOverlapEnd);
	}
}
ACardkeyDoorBody::~ACardkeyDoorBody()
{
}

void ACardkeyDoorBody::BeginPlay()
{
	Super::BeginPlay();

	// ��탌�o�[�擾�A�ۑ�
	TSubclassOf<AAutomaticDoorLever> findClass;
	findClass = AAutomaticDoorLever::StaticClass();
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, actors);

	if (actors.Num() > 0)
	{
		for (int idx = 0; idx < actors.Num(); ++idx)
		{
			AAutomaticDoorLever* p_lever = Cast<AAutomaticDoorLever>(actors[idx]);

			// ��탌�o�[�Ȃ�ۑ�
			if (p_lever->ActorHasTag("AlertLever"))
			{
				p_alert_lever_ = p_lever;
			}
		}
	}

	if (p_door_state_material_ != NULL)
	{
		p_material_instance_dynamic_ = p_door_state_mesh_1_->CreateAndSetMaterialInstanceDynamicFromMaterial(0, p_door_state_material_);
		if (p_material_instance_dynamic_ != NULL)
		{
			p_door_state_mesh_1_->SetMaterial(0, p_material_instance_dynamic_);
			p_door_state_mesh_2_->SetMaterial(0, p_material_instance_dynamic_);
			p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
		}
		else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("p_material_instance_dynamic_ is NULL."));
	}
	else	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("p_door_state_material_ is NULL."));	

	// �h�A�̏����ʒu���擾
	left_door_start_posY_ = p_leftdoor_mesh_->GetRelativeLocation().Y;
	right_door_start_posY_ = p_rightdoor_mesh_->GetRelativeLocation().Y;

	// �h�A�̎n�_����I�_�܂ł̋������v�Z
	distance_start_to_end_ = FMath::Abs(left_door_end_posY_ - left_door_start_posY_);

	// ���h�A�̊J���Ƃ��̈ړ�������ݒ�(�E�h�A�͂���𔽓]���ė��p)
	left_door_dire_for_move_ = FMath::Abs(left_door_end_posY_) / left_door_end_posY_;

	p_player_character_ = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (p_player_character_ == NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("PlayerCharacter is None."));
	}

	// �A�C�e���q���g�𐶐����� (�쐬��:�щ_��)
	bp_ItemInfo = TSoftClassPtr<AActor>(FSoftObjectPath("Blueprint'/Game/Blueprints/BP_ItemHint3D.BP_ItemHint3D_C'")).LoadSynchronous();	// path�ɂ���N���X���擾
	if (bp_ItemInfo != nullptr && isNeedToDiaplsy == true)
	{
		itemInfoActor = GetWorld()->SpawnActor<AActor>(bp_ItemInfo);						// CardKey Reader1�̃A�C�e���q���g��Actor�Ƃ��Đ�������
		itemInfoActor2 = GetWorld()->SpawnActor<AActor>(bp_ItemInfo);						// CardKey Reader2�̃A�C�e���q���g��Actor�Ƃ��Đ�������

		// ��ڃJ�[�h�L�[���[�_�[
		if (itemInfoActor != NULL)
		{

			itemInfoActor->SetActorEnableCollision(false);

			if (p_cardreader_mesh_1_ != NULL)
			{
				itemInfoActor->SetActorLocation((p_cardreader_mesh_1_->GetComponentLocation() + infoPosition));
			} // end if()

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
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor is not valid (CardkeyDoorBody)"));
		} // end else

		// ��ڃJ�[�h�L�[���[�_�[
		if (itemInfoActor2 != NULL)
		{

			itemInfoActor2->SetActorEnableCollision(false);

			if (p_cardreader_mesh_2_ != NULL)
			{
				itemInfoActor2->SetActorLocation((p_cardreader_mesh_2_->GetComponentLocation() + infoPosition2));
			} // end if()

			FOutputDeviceNull ar;
			FString FuncName_and_Solution1 = FString::Printf(TEXT("InitialHeight "));
			FString FuncName_and_Solution2 = FString::Printf(TEXT("InitialScale "));
			FString FuncName_and_Solution3 = FString::Printf(TEXT("InitialDistance "));

			FuncName_and_Solution1 += FString::SanitizeFloat(infoWorkingHeight);
			FuncName_and_Solution2 += FString::SanitizeFloat(infoScale);
			FuncName_and_Solution3 += FString::SanitizeFloat(toPlayers_MinDistance);

			itemInfoActor2->CallFunctionByNameWithArguments(*FuncName_and_Solution1, ar, NULL, true);
			itemInfoActor2->CallFunctionByNameWithArguments(*FuncName_and_Solution2, ar, NULL, true);
			itemInfoActor2->CallFunctionByNameWithArguments(*FuncName_and_Solution3, ar, NULL, true);

		} // end if()
		else {
			UE_LOG(LogTemp, Log, TEXT("itemInfoActor2 is not valid (CardkeyDoorBody)"));
		} // end else
	} // end if()
	else {
		UE_LOG(LogTemp, Log, TEXT("item hint BP is not exist (CardkeyDoorBody)"));
	} // end else

}

void ACardkeyDoorBody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAlertLeverState();

	UpdateDoorState(DeltaTime);

	UpdateDoorMove(DeltaTime);
}

void ACardkeyDoorBody::CheckAlertLeverState()
{
	if (is_alert_lever_on_)	return;

	if (p_alert_lever_ != NULL)
	{
		is_alert_lever_on_ = p_alert_lever_->GetLeverState();

		if (is_alert_lever_on_)
		{
			door_state_ = kDoorStateOpening;
		}
	}
}

void ACardkeyDoorBody::UpdateDoorState(float _deltatime)
{
	switch (door_state_)
	{
	case kDoorStateLoading:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= check_cardkey_time_)
		{			
			count_for_check_display_time_ = 0.0f;
			if ((p_player_character_->GetHaveCardkeyState() & (1 << door_filter_num_)) != 0)
			{
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_success_);

				if (p_sound_loading_success_ != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_loading_success_, GetActorLocation());

					// �h�A���J������SE��炷
					if (p_sound_door_open_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_open_, GetActorLocation());
				}
				door_state_ = kDoorStateOpening;
			}
			else
			{
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_error_);

				if (p_sound_loading_error_ != NULL)
				{
					UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_loading_error_, GetActorLocation());
				}
				door_state_ = kDoorStateError;
			}
		}
		break;

	case kDoorStateError:
		count_for_check_display_time_ += _deltatime;
		if (count_for_check_display_time_ >= display_error_time_)
		{
			count_for_check_display_time_ = 0.0f;
				if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
				door_state_ = kDoorStateClosed;
			
		}
		break;

	case kDoorStateOpened:
		CheckDetectSpan(_deltatime);
		break;

	case kDoorStateClosed:
		break;

	case kDoorStateClosing:
		if (is_doorbody_eventbox_overlap_)
		{
			door_state_ = kDoorStateOpening;
		}

		if (required_time_for_open_and_close_ <= 0.0f)
		{
			door_state_ = kDoorStateClosed;
		}
		break;

	case kDoorStateOpening:
		if (required_time_for_open_and_close_ >= open_and_close_time_)
		{
			door_state_ = kDoorStateOpened;
		}
		break;

	default:
		break;
	}
}

void ACardkeyDoorBody::UpdateDoorMove(float _deltatime)
{
	switch (door_state_)
	{
	case kDoorStateOpened:
	case kDoorStateLoading:
	case kDoorStateError:
	case kDoorStateClosed:
		return;
		break;

	case kDoorStateClosing:
		open_time_count_ = 0.0f;
		required_time_for_open_and_close_ -= _deltatime;
		if (required_time_for_open_and_close_ < 0.0f) { required_time_for_open_and_close_ = 0.0f; }
		break;

	case kDoorStateOpening:
		required_time_for_open_and_close_ += _deltatime;
		if (required_time_for_open_and_close_ > open_and_close_time_) { required_time_for_open_and_close_ = open_and_close_time_; }
		break;

	default:
		return;
		break;
	}

	// ���h�A�̏���
	float newLocationY = left_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_);
	p_leftdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));

	// �E�h�A�̏���
	newLocationY = right_door_start_posY_ + ((distance_start_to_end_ * required_time_for_open_and_close_) * left_door_dire_for_move_ * -1.0f);
	p_rightdoor_mesh_->SetRelativeLocation(FVector(0, newLocationY, 0));
}

void ACardkeyDoorBody::CheckDetectSpan(float _deltatime)
{
	if (is_alert_lever_on_)	return;

	open_time_count_ += _deltatime;

	// ���߂����Ԃ𒴂�����`�F�b�N
	if (open_time_count_ > detect_span_)
	{
		// ���m�ł��Ȃ���Ώ�Ԃ�CLOSING��
		if (is_doorbody_eventbox_overlap_)
		{
			// �J�������p���̂��߂̐��l���
			open_time_count_ = 0.0f;
		}
		else
		{
			if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_standby_);
			door_state_ = kDoorStateClosing;

			// �h�A�����鎞��SE��炷
			if (p_sound_door_close_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), p_sound_door_close_, GetActorLocation());
		}
	}
}

void ACardkeyDoorBody::CheckedByPlayer()
{
	if (door_state_ == kDoorStateClosed)
	{
		// �J�[�h�ǂݍ��݉���炷
		if (sound_when_checked_ != NULL)	UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound_when_checked_, GetActorLocation());
		
		if (p_material_instance_dynamic_ != NULL)	p_material_instance_dynamic_->SetVectorParameterValue(TEXT("door_state_color"), state_color_loading_);
		door_state_ = kDoorStateLoading;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapBegin(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComponent, int32 _otherBodyIndex, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// ���m���̃A�N�^�[�����C���N�������g
		++door_eventbox_overlap_sum_;

		// ���ݖh�~�p�t���O�𗧂Ă�
		is_doorbody_eventbox_overlap_ = true;
	}
}

void ACardkeyDoorBody::OnDoorBodyOverlapEnd(UPrimitiveComponent* _overlappedComponent, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIndex)
{
	if (_otherActor->ActorHasTag("Player") || _otherActor->ActorHasTag("Enemy"))
	{
		// ���m���̃A�N�^�[�����f�N�������g
		--door_eventbox_overlap_sum_;

		// ���m���̃A�N�^�[��0�ȉ��Ȃ狲�ݖh�~�p�t���O������
		if (door_eventbox_overlap_sum_ <= 0)		is_doorbody_eventbox_overlap_ = false;
	}
}