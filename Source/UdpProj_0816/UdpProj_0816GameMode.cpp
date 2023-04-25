// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdpProj_0816GameMode.h"
#include "Net/NetCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

AUdpProj_0816GameMode::AUdpProj_0816GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
//
//void AUdpProj_0816GameMode::RequestRespawn(class ANetCharacter* NetPlayer, AController* PlayerController)
//{
//	if (NetPlayer)
//	{
//		NetPlayer->Reset();
//		NetPlayer->Destroy();
//	}
//	if (PlayerController)
//	{
//		// 获取场景中的PlayerStart。随机选中一个，然后作为复活点
//		TArray<AActor*> HubPlayerStarts;
//		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(),HubPlayerStarts);
//		int32 Selection = FMath::RandRange(0,HubPlayerStarts.Num() -1);
//
//		RestartPlayerAtPlayerStart(PlayerController, HubPlayerStarts[Selection]);
//	}
//}
