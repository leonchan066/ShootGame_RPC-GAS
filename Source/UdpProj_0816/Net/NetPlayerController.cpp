// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"


/*
ANetPlayerController::ANetPlayerController(const FObjectInitializer&amp; ObjectInitializer) : Super(ObjectInitializer)
{
	PawnToUseA = NULL;
	PawnToUseB = NULL;

	bReplicates = true;
}

void ANetPlayerController::DeterminPawnClass_Implementation()
{
	if (IsLocalController())
	{
		TArray TextStrings;
		const FString FilePath = FPaths::GameDir() + "Textfiles/PlayerSettings.txt";

		if (TextStrings[0] == "PawnA")
		{
			ServerSetPawn(PawnToUseA);
			return;
		}
		ServerSetPawn(PawnToUseB);
		return;
	}
}

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	DeterminPawnClass();
}

void ANetPlayerController::ServerSetPawn_Implementation(TSubclassOf InPawnClass)
{
	MyPawnClass = InPawnClass;

	GetWorld()->GetAuthGameMode()->RestartPlayer(true);
}

void ANetPlayerController::ServerSetPawn_Validate(TSubclassOf InPawnClass)
{
	return true;
}

void ANetPlayerController::GetLifetimeReplicatedProps(TArray&amp; OutLifetimeProps) const
{
	DOREPLIFETIME(AMyPlayerController, MyPawnClass);
}
*/