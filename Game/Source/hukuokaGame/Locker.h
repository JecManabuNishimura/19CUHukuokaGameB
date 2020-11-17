//-------------------------------------------------------------------
// �t�@�C��		�FLocker.h
// �T�v			�F���b�J�[�̐���
// �쐬��		�F19CU0209 ���葓��
// �쐬��		�F2020/11/16
//-------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Locker.generated.h"

UCLASS()
class HUKUOKAGAME_API ALocker : public AItemBase
{
	GENERATED_BODY()
	
public:

	// �R���X�g���N�^
	ALocker();

	// �f�X�g���N�^
	~ALocker();

protected:
	// ���s���Ɉ�x�Ă΂��
	virtual void BeginPlay() override;

public:
	// ���t���[���Ă΂��
	virtual void Tick(float DeltaTime) override;

public:
	// ���b�V���֌W
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* body_mesh_;		// �{�̂̃��b�V��
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* door_mesh_;		// ���̃��b�V��

	// �l
	UPROPERTY(EditAnywhere)
		int max_rotation;					// ���x�܂ŊJ����
	UPROPERTY(EditAnywhere)
		int open_and_close_frame;			// �J���E����Ƃ��ɂ�����t���[����
	UPROPERTY(EditAnywhere)
		int player_change_rotation_frame;	// ���b�J�[���܂������Ƀv���C���[�̎��_���ς�鎞�̃t���[����
	UPROPERTY(EditAnywhere)
		int player_to_locker_frame;			// �v���C���[�����b�J�[�ɓ���E�o�鎞�̃t���[����

	// �֐�
		// �v���C���[�Ƀ`�F�b�N���ꂽ��Ă΂��(�쓮)
	virtual void CheckedByPlayer();

	void GetPlayer();

private:
	APlayerCharacter* player;

	bool is_end_rotation_;		// ��]�����I�������ǂ���
	bool can_input_;			// ���͉\���ǂ���
	bool is_check_;				// �v���C���[�����ׂ���
	bool is_move_in_locker_;		// ���b�J�[�Ɉړ������ǂ���
	bool is_in_player_;			// �J��->����̎���true�ɂ���
	bool player_rotation_start_flag_;	// �v���C���[�̉�]���J�n�����邩�ǂ����̃t���O
	float add_rotation_value_;	// 1�t���[�����Ƃɂǂꂾ��������]�����邩
	float now_rotation_value_;	// ���݂̊p�x 
	float add_player_rotation_value_; // 1�t���[�����Ƃɂǂꂾ���v���C���[����]�����邩
	FVector locker_body_location_;	// ���b�J�[�̖{�̂̈ʒu
	FVector player_location_save_;	// ���b�J�[�ɓ��낤�Ƃ����Ƃ���Player�̍��W�i�[�p(���b�J�[����o�鎞�ɕK�v)
	FVector player_move_vector_;		// 1�t���[�����ƂɃv���C���[���ǂꂾ���i�ނ����i�[
	FRotator locker_body_rotation_;	// ���b�J�[�{�̂̉�]
	float location_lerp_alpha;
	float location_add_lerp_value;
	float rotation_lerp_alpha;
	float rotation_add_lerp_value;
	float def_door_rotation;

	void SetDoorRotationValue();
	void UpdateDoorRotation();
	void InToLocker();
	void OutToLocker();
	void CloseDoor();
	void PlayerRotation();
};