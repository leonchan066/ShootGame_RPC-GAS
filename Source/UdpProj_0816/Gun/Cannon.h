// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UdpProj_0816/RwFiles.h"
#include "Cannon.generated.h"

UCLASS()
class UDPPROJ_0816_API ACannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon();

	void GetLifetimeReplicatedProps( TArray<class FLifetimeProperty> & OutLifetimeProps ) const override;

	UFUNCTION()
	float GetCurrentHealth();

	// Damage System
	UFUNCTION()
		void SetCurrentHealth(float healthValue);

	UFUNCTION()
		void SetCurrentLevel(int levelValue);

	UFUNCTION()
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	URwFiles* RwIni;

/// <summary>
/// Cannon Fire Bullet
/// </summary>

	UFUNCTION(BlueprintCallable)
	void CannonFireBullet();

	UFUNCTION(Client, Reliable)
	void FireBullet(float factor = 1.0f);

/// <summary>
/// 子弹生成位置以及属性（使用蓝图设置
/// </summary>

	UPROPERTY(EditAnywhere, Category = "CannonBullet")
	FVector BulletLocation;

	UPROPERTY(EditAnywhere, Category = "CannonBullet")
	FVector BulletRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay | Projectile")
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable )
	void MuzzleLoc(FVector loc);


/// <summary>
/// 炮台属性
/// </summary>

	// Level
	UPROPERTY(EditAnywhere,Category = "CannonLevel")
	int MaxLevel;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel)
	int CurrentLevel;

	UFUNCTION()
	void OnRep_CurrentLevel();

	void OnLevelUpdate();


	// Health
	UPROPERTY(EditAnywhere,Category="CannonHealth")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing= OnRep_CurrentHealth)
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void OnHealthUpdate();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
