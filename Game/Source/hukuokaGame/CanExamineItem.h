//-------------------------------------------------------------------
// �t�@�C��		�FCanExamineItem.h
// �T�v			�FItemBase�N���X���p������t�@�C�����̒��ׂ���A�C�e���N���X
// �쐬��		�F19CU0233 ����I�l
// �쐬��		�F2020/10/26
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "CanExamineItem.generated.h"

// �C�x���g�f�B�X�p�b�`���[�錾
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFileTrunPageEventDispatcher);		// �v���C���[�̎�����CanCheck�A�C�e���ɓ���������

// �t�@�C���̍\����(string�^10��)
USTRUCT(BlueprintType)
struct FFileTextStruct: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FFileTextStruct() {}

public:
	UPROPERTY(EditAnywhere)
		FString page1;

	UPROPERTY(EditAnywhere)
		FString page2;

	UPROPERTY(EditAnywhere)
		FString page3;

	UPROPERTY(EditAnywhere)
		FString page4;

	UPROPERTY(EditAnywhere)
		FString page5;

	UPROPERTY(EditAnywhere)
		FString page6;

	UPROPERTY(EditAnywhere)
		FString page7;

	UPROPERTY(EditAnywhere)
		FString page8;

	UPROPERTY(EditAnywhere)
		FString page9;

	UPROPERTY(EditAnywhere)
		FString page10;
};

UCLASS()
class HUKUOKAGAME_API ACanExamineItem : public AItemBase
{
	GENERATED_BODY()

public:
	ACanExamineItem();

protected:
	virtual void BeginPlay() override;

private:
	// �v���C���[
	APlayerCharacter* player_character_;

	// �t�@�C���̃X�P���^�����b�V��
	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* file_mesh_;

	UPROPERTY(EditAnyWhere)
		UDataTable* datatable_;			// �t�@�C���̃e�L�X�g�f�[�^

	FTransform transform_on_map_;		// �}�b�v��ɔz�u����Ă��鎞�̏��(���׏I�����Ƃ��ɗ��p)

	FLinearColor text_linear_color_;	// �e�L�X�g�̃J���[

	TArray<FString> text_in_file_kind_;	// �w�肵���t�@�C���ԍ��̑S�e�L�X�g�f�[�^�i�[�p�ϐ� 

	FString left_text_;					// ���y�[�W�ɕ\�����镶��

	FString right_text_;				// �E�y�[�W�ɕ\�����镶��

	UPROPERTY(EditAnywhere, meta = (ClampMin = 1))
		int file_kind_;					// �t�@�C���̎��ʔԍ�

	int page_num_;						// ���y�[�W��

	int left_page_open_now_num_;		// �\�����̍��y�[�W��

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_open_close_;			// ���ׂĂ���A�j���[�V�������n�߂�܂ł̕b��

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0))
		float time_display_text_;		// ���ׂĂ���e�L�X�g���t�F�[�h�C�����n�߂�܂ł̕b��

	UPROPERTY(EditAnywhere)
		float distance_from_file_to_player_;	// ���ׂ��Ƃ��̃v���C���[����t�@�C���܂ł̋���

	float count_for_time_open_close_;	// �A�j���[�V�����A�e�L�X�g�t�F�[�h�C���Ǘ��p�̃J�E���g

	bool is_show_details_;				// �v���C���[�Ƀ`�F�b�N����Ă��邩�̃t���O

	bool can_start_anim_;				// �A�j���[�V�����J�n���̃t���O

	bool can_turn_page_;				// �y�[�W���߂��鋖�t���O

	bool can_show_detial_;				// �߂����Ƃ���Ƃ��ɍēx���ׂĂ��܂��̂�h�~���邽�߂̃t���O

	bool do_file_loc_correction_;		// �t�@�C���ʒu�␳�p�t���O

	void TurnPage();

	UPROPERTY(BlueprintAssignable)
		FOnFileTrunPageEventDispatcher  OnFileTrunPageEventDispatcher;
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
		UWidgetComponent* widget_comp_;	// �G�f�B�^��Ő����������̂��擾

	// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮)
	void CheckedByPlayer();

	// �t�@�C���̎��ʔԍ���Ԃ�
	UFUNCTION(BlueprintCallable)
		int ReturnFileKind() { return file_kind_; }

	// �A�j���[�V�����J�n���t���O��Ԃ�
	UFUNCTION(BlueprintCallable)
		bool ReturnCanStartAnim() { return can_start_anim_; }

	// �e�L�X�g�̐F��Ԃ�
	UFUNCTION(BlueprintCallable)
		FLinearColor ReturnTextColor() { return text_linear_color_; }

	// ���y�[�W�ɕ\������e�L�X�g��Ԃ�
	UFUNCTION(BlueprintCallable)
		FString ReturnLeftPageText() { return left_text_; }

	// �E�y�[�W�ɕ\������e�L�X�g��Ԃ�
	UFUNCTION(BlueprintCallable)
		FString ReturnRightPageText() { return right_text_; }
};
