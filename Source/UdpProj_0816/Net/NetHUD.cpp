// Fill out your copyright notice in the Description page of Project Settings.


#include "NetHUD.h"
#include "Engine/Canvas.h"
#include "UdpProj_0816/Net/NetCharacter.h"

void ANetHUD::DrawHUD()
{
	Super::DrawHUD();

	ANetCharacter* PlayerChar = Cast<ANetCharacter>(GetOwningPawn());
	if (PlayerChar)
	{

		//Canvas->DrawText(GEngine->GetLargeFont(),FString::Printf(TEXT("You have been killed %d Enemies"), PlayerChar->KillCounts),300,10);
	}

}


