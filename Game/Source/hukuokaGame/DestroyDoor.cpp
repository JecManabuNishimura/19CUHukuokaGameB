//-------------------------------------------------------------------
// ファイル		：DestroyDoor.h
// 概要			：赤ちゃんが泣いたとき、敵が突き破るドアの処理の作成
// 作成者		：19CU0209 尾崎蒼宙
// 作成日		：2020/12/10
//-------------------------------------------------------------------


#include "DestroyDoor.h"

// Sets default values
ADestroyDoor::ADestroyDoor()
	: m_is_baby_cry(false)
	, m_pdoor_mesh_(NULL)
	, m_impulsevalue_(FVector::ZeroVector)
	, m_time_cnt_(0.f)
	, m_impuls_flag_(false)
	, m_disappea_collision_time_(0.f)
	, glass_se_(NULL)
	, door_se_(NULL)
	, root(NULL)
	, flag_(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	if (root)
	{
		RootComponent = root;
	}

	// m_pdoor_mesh_の作成
	m_pdoor_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("m_pdoor_mesh_"));
	if (m_pdoor_mesh_)
	{
		m_pdoor_mesh_->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void ADestroyDoor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADestroyDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_is_baby_cry)
	{	m_pdoor_mesh_ ->SetSimulatePhysics(true);
		m_pdoor_mesh_->AddImpulse(m_impulsevalue_);
		m_is_baby_cry = false;
		flag_ = true;
		m_pdoor_mesh_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		m_impuls_flag_ = true;
	}
	
	if(m_impuls_flag_)
	{
		if (m_disappea_collision_time_ == 0)
		{
			return;
		}

		m_time_cnt_ += DeltaTime;
		if (m_time_cnt_ >= m_disappea_collision_time_)
		{
			m_pdoor_mesh_->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
			m_disappea_collision_time_ = 0;
			m_impuls_flag_ = false;
		}
	}
}

