// Fill out your copyright notice in the Description page of Project Settings.


#include "RwFiles.h"


FString URwFiles::ReadTxt(FString path)
{
	FString resultString;

	FFileHelper::LoadFileToString(resultString, *(FPaths::ProjectContentDir() + path));

	return resultString;
}

bool URwFiles::WriteTxt(FString saveStr, FString path)
{
	bool result;
	result = FFileHelper::SaveStringToFile(saveStr, *(FPaths::ProjectContentDir() + path));
	return result;
}

void URwFiles::ReadIniValue(FString Sectoin, FString inKey, FString& outValue)
{
	if (!GConfig)
		return;

	GConfig->GetString(*Sectoin, *inKey, outValue, GGameIni);

}


bool URwFiles::WriteIni(FString newSection, FString newKey, FString newValue)
{
	if (!GConfig)
		return false;

	GConfig->SetString(*newSection, *newKey, *newValue, GGameIni);
	return true;

}


//////////////////////////////////////////////////////////////////////////



FString URwFiles::GetIniValue(FString Section, FString inKey)
{
	if (!GConfig)
		return "";
	FString outValue = "";
	ReadIniValue(Section,inKey,outValue);
	return outValue;
}


float URwFiles::GetIniFloat(FString Section, FString inKey)
{
	if (!GConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("ERROOOOOOOOOOR read Ini Value. Return 0.f"));
		return 0.f;
	}
	FString outValue = "";
	ReadIniValue(Section, inKey, outValue);
	return FCString::Atof(*outValue);
}

void URwFiles::SetIniValue(FString Section, FString inKey, FString newValue)
{

}