// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "UdpProj_0816/Net/NetPlayerController.h"
#include "UdpProj_0816/Net/NetHUD.h"

ANetGameModeBase::ANetGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = ANetHUD::StaticClass();
}

void ANetGameModeBase::RequestRespawn(class ANetCharacter* NetPlayer, AController* PlayerController)
{
	if (NetPlayer)
	{
		NetPlayer->Reset();
		NetPlayer->Destroy();
	}
	if (PlayerController)
	{
		// 获取场景中的PlayerStart。随机选中一个，然后作为复活点
		TArray<AActor*> HubPlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), HubPlayerStarts);
		int32 Selection = FMath::RandRange(0, HubPlayerStarts.Num() - 1);

		RestartPlayerAtPlayerStart(PlayerController, HubPlayerStarts[Selection]);
	}
}
/*
UCLASS* ANetGameModeBase::GetDefaultPawnClassForController(AController* InController)
{
	ANetPlayerController* NetController = Cast(InController);
	if (NetController)
	{
		return NetController->GetPlayerPawnClass();
	}

	return DefaultPawnClass;
}
*/