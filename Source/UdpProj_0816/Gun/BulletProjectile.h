// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UdpProj_0816/RwFiles.h"
#include "BulletProjectile.generated.h"

UCLASS()
class UDPPROJ_0816_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DamageValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float RecoveryValue;

	UPROPERTY()
	float factorValue;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;


	UFUNCTION(Category = "ProjectileProtected")
	void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit);

	URwFiles* RwIni;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
