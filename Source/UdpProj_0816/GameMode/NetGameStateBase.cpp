// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameStateBase.h"

ANetGameStateBase::ANetGameStateBase()
{

}

void ANetGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("NetGameStateBase: Start: %f"), GetServerWorldTimeSeconds());

	UE_LOG(LogTemp, Warning, TEXT("NetGameStateBase: Start: %d"), HasBegunPlay());

}

void ANetGameStateBase::Tick(float DeltaTime)
{
}
