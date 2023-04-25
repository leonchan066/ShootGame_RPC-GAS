// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NetHUD.generated.h"

/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API ANetHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	
};
