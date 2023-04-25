// Fill out your copyright notice in the Description page of Project Settings.


#include "BonusBoxArea.h"

// Sets default values
ABonusBoxArea::ABonusBoxArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	//RootComponent = RootScene;

	//StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	//StaticMesh->SetupAttachment(RootComponent);
	//StaticMesh->SetCollisionProfileName(FName("CubeProfile"));

}

// Called when the game starts or when spawned
void ABonusBoxArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABonusBoxArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABonusBoxArea::NotifyActorBeginOverlap(AActor* OtherActor)
{
	auto hubActor = Cast<ACharacter>(OtherActor);
	if (hubActor != nullptr)
	{
		UE_LOG(LogTemp,Warning, TEXT("Character Enter"));
	}
}

void ABonusBoxArea::NotifyActorEndOverlap(AActor* OtherActor)
{
	auto hubActor = Cast<ACharacter>(OtherActor);
	if (hubActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Exit"));
	}
}