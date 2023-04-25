// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpProj_0816/UdpProj_0816Character.h"
#include "GameFramework/Actor.h"
#include "BonusBoxArea.generated.h"

UCLASS()
class UDPPROJ_0816_API ABonusBoxArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABonusBoxArea();

	//	Actor Enter
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	//	Actor Exit
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class USceneCaptureComponent* RootScene;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
