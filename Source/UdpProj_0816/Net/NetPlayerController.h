// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()

	/*
public:
	ANetPlayerController(const FObjectInitializer&amp; ObjectInitializer);

	FORCEINLINE UCLASS* GetPlayerPawnClass() { return NetPawnPlayer; };

protected:
	UFUNCTION(Reliable,Client)
	void DeterminPawnClass();
	virtual void DeterminPawnClass_Implementation();

	virtual void BeginPlay() override;

	UFUNCTION(Reliable, Server, WithValidation)
	virtual void ServerSetPawn(TSubclassOf InPawnClass);
	virtual void ServerSetPawn_Implementation(TSubclassOf InPawnClass);
	virtual void ServerSetPawn_Validate(TSubclassOf InPawnClass);

	UPROPERTY(Replicated)
	TSubclassOf MyPawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyNetController")
	TSubclassOf PawnToUseA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyNetController")
	TSubclassOf PawnToUseB;
	*/
	


};