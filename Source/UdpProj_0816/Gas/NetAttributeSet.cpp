// Fill out your copyright notice in the Description page of Project Settings.


#include "NetAttributeSet.h"
// 以下两个是用来自定义GE伤害计算所必须的
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"

UNetAttributeSet::UNetAttributeSet()//:Health(100.f)
{
	Health = RwIni->GetIniFloat(TEXT("PlayerAttr"), TEXT("MaxHealth"));
}



FGameplayAttributeData UNetAttributeSet::GetHealth()
{
	return Health;
}

void UNetAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UNetAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UNetAttributeSet, Health)))
	{
		UE_LOG(LogTemp,Warning, TEXT("My Health Base is %f"), Health.GetBaseValue());
		UE_LOG(LogTemp,Warning, TEXT("My Health Now is %f"), Health.GetCurrentValue());
	}
}

void UNetAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNetAttributeSet, Health, COND_None,REPNOTIFY_Always);
	//DOREPLIFETIME(UNetAttributeSet, Health );
}

void UNetAttributeSet::OnRep_Health(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetAttributeSet, Health, OldMaxHealth);
}
