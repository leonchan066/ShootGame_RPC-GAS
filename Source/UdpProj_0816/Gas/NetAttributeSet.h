// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UdpProj_0816/RwFiles.h"


#include "NetAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class UDPPROJ_0816_API UNetAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UNetAttributeSet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttrSet",ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNetAttributeSet,Health);

	URwFiles* RwIni;

	UFUNCTION()
	FGameplayAttributeData GetHealth();


	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
	
	virtual void GetLifetimeReplicatedProps( TArray<class FLifetimeProperty> & OutLifetimeProps ) const override;

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldMaxHealth);

};
