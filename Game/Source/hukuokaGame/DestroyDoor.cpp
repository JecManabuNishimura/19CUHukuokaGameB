// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyDoor.h"

// Sets default values
ADestroyDoor::ADestroyDoor()
	: m_is_baby_cry(false)
	, m_pdoor_mesh_(NULL)
	,m_Impulsevalue(FVector::ZeroVector)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// m_pdoor_mesh_�̍쐬
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
		m_pdoor_mesh_->AddImpulse(m_Impulsevalue);
		m_is_baby_cry = false;
	}
}

