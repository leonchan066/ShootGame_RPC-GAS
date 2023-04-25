// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API ANetGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANetGameStateBase();

	virtual void Tick(float DeltaTime) override;


protected:
	virtual void BeginPlay() override;
	

};
