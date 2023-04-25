// Fill out your copyright notice in the Description page of Project Settings.


#include "UdpProj_0816/Gun/Cannon.h"
#include "UdpProj_0816/Net/NetCharacter.h"
#include "BulletProjectile.h"

#include "UdpProj_0816/GameMode/NetGameStateBase.h"
#include "Net/UnrealNetwork.h"	
#include "Engine/Engine.h"		// including "AddOnscreenDebugMessage"


// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	CurrentLevel = RwIni->GetIniFloat(TEXT("CannonAttr"), TEXT("BaseLevel")); 
	MaxLevel = RwIni->GetIniFloat(TEXT("CannonAttr"),TEXT("MaxLevel"));
	MaxHealth = RwIni->GetIniFloat(TEXT("CannonAttr"), TEXT("MaxHealth"));
	CurrentHealth = MaxHealth;
	ProjectileClass = ABulletProjectile::StaticClass();
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/// <summary>
/// Cannon发射子弹
/// </summary>

void ACannon::MuzzleLoc(FVector loc)
{
	BulletLocation = loc;
}

void ACannon::CannonFireBullet()
{
	FireBullet(1.f);
}

void ACannon::FireBullet_Implementation(float factor)
{

	// 调用Character的Fire函数，确保子弹可以被服务器执行（如果直接让Cannon发射，在攻击到Ai后，SetCurrentHealth会无法通过 ROLE_Authority,因为会判断为 Client

	auto HubOwnerActor = Cast<ANetCharacter>(GetOwner());

	float DamageFactor = CurrentLevel / 10.f;

	HubOwnerActor->OtherFire(1, DamageFactor * 0.5f , BulletLocation, GetActorRotation());
	// Cannon 的伤害是 Player 的BaseAck 的一半

}



//////////////////////////////////////////////////////////////////////////
///	血量，等级的同步
//////////////////////////////////////////////////////////////////////////

void ACannon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACannon, CurrentHealth);
	DOREPLIFETIME(ACannon, CurrentLevel);
}

//////////////////////////////////////////////////////////////////////////
/// health of cannon
//////////////////////////////////////////////////////////////////////////

float ACannon::GetCurrentHealth()
{
	return CurrentHealth;
}


void ACannon::OnHealthUpdate()
{
	//return;
	// Client
	if (HasAuthority() == false)
	{
		FString healthMessage = FString::Printf(TEXT("Cannnon CurrentHealth: %f"), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("Cannon was killed"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	// Server
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining"), *GetFName().ToString() , CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, healthMessage);
	}


	///
	
	///
}

void ACannon::OnRep_CurrentHealth()
{

	OnHealthUpdate();
	if (CurrentHealth <= 0)
	{
		Destroy();
	}
}

void ACannon::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue,0.f,MaxHealth);

		OnHealthUpdate();
		if (CurrentHealth <= 0)
		{
			Destroy();
		}
	}
}

/// <summary>
/// 处理伤害
/// </summary>
/// <param name="HubAmount"></param>
/// <param name="DamageEvent"></param>
/// <param name="EventInstigator"></param>
/// <param name="DamageCauser"></param>
/// <returns></returns>

float ACannon::TakeDamage(float HubAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	auto HubCauser = Cast<ABulletProjectile>(DamageCauser);
	if (HubCauser)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%d"), HubCauser.);
	}
	float damageApplied = CurrentHealth - HubAmount;
	if (DamageCauser->GetOwner() != GetOwner())
	{		
		SetCurrentHealth(damageApplied);
	}
	else 
	{
		float levelApplied = CurrentLevel + HubAmount;
		SetCurrentLevel(levelApplied);
	}
	return damageApplied;
}

//////////////////////////////////////////////////////////////////////////
/// level of cannon
//////////////////////////////////////////////////////////////////////////

void ACannon::SetCurrentLevel(int levelValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentLevel = FMath::Clamp(levelValue, 1, MaxLevel);
		OnLevelUpdate();
	}
}

void ACannon::OnRep_CurrentLevel()
{
	OnLevelUpdate();
	UE_LOG(LogTemp, Warning, TEXT(" Abc Level : %d"), CurrentLevel);
}



void ACannon::OnLevelUpdate()
{
	return;
}

