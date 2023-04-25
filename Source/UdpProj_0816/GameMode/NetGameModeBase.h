// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UdpProj_0816/Net/NetCharacter.h"
#include "NetGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API ANetGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANetGameModeBase();
	virtual void RequestRespawn(class ANetCharacter* NetPlayer, AController* PlayerController);
	/*
	UCLASS* GetDefaultPawnClassForController(AController* InController) override;
	*/
};
