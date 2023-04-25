// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RwFiles.generated.h"

/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API URwFiles : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//	in:	The path of txt File
	UFUNCTION(BlueprintCallable, Category = "Read and Write File")
		static FString ReadTxt(FString path);

	//	in:	1. What you want to save
	//	in:	2. The path of txt File
	UFUNCTION(BlueprintCallable, Category = "Read and Write File")
		static bool WriteTxt(FString saveStr, FString path);

	UFUNCTION(BlueprintCallable, Category = "Read and Write File")
		static void ReadIniValue(FString Section, FString inKey, FString& outValue);

	UFUNCTION(BlueprintCallable, Category = "Read and Write File")
		static bool WriteIni(FString newSection, FString newKey, FString newValue);

	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Read and Write File")
		static	FString GetIniValue(FString Section, FString inKey);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Read and Write File")
		static	float GetIniFloat(FString Section, FString inKey);

	UFUNCTION(BlueprintCallable, Category = "Read and Write File")
		static void SetIniValue(FString Section, FString inKey, FString newValue);
		
};
