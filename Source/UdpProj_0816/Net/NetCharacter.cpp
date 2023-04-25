// Fill out your copyright notice in the Description page of Project Settings.


#include "NetCharacter.h"
#include "UdpProj_0816/GameMode/NetGameStateBase.h"

#include "Net/UnrealNetwork.h"	
#include "Engine/Engine.h"		// including "AddOnscreenDebugMessage"


#include "UdpProj_0816/Gun/BulletProjectile.h"
#include "UdpProj_0816/Gamemode/NetGameModeBase.h"



// Sets default values
ANetCharacter::ANetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	leonASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("leonASC"));
	NetAttrSetComp = CreateDefaultSubobject<UNetAttributeSet>(TEXT("NetAttrSet"));


	KillCounts = 0;
	///	获取玩家生命值
	//TODO: Replace using RwIni
	MaxHealth = RwIni->GetIniFloat(TEXT("PlayerAttr"),TEXT("MaxHealth"));
	CurrentHealth = MaxHealth;

	// 发射
	ProjectileClass = ABulletProjectile::StaticClass();
	// 初始化射速
	FireRate = 0.25f;
	bIsFiringWeapon = false;
}



// Called when the game starts or when spawned
void ANetCharacter::BeginPlay()
{
	Super::BeginPlay();



	if (leonASC == nullptr)
		return;

	if (HasAuthority() && GameplayAbilities.Num()) {

		for (auto i = 0; i < GameplayAbilities.Num(); i++)
		{
			if (GameplayAbilities[i] == nullptr)
			{
				continue;
			}
			leonASC->GiveAbility(FGameplayAbilitySpec(GameplayAbilities[i].GetDefaultObject(), 1, 0));
		}
	}
	leonASC->InitAbilityActorInfo(this, this);


	/// <summary>
	/// 注册委托
	/// </summary>
	if (leonASC)
	{
		HealthChangedDelegateHandle = leonASC->GetGameplayAttributeValueChangeDelegate(NetAttrSetComp->GetHealthAttribute()).AddUObject(this,&ANetCharacter::HealthChanged);
	}
}

// Called every frame
void ANetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	
}

// Called to bind functionality to input
void ANetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* ANetCharacter::GetAbilitySystemComponent() const
{
	return leonASC;
}

//////////////////////////////////////////////////////////////////////////
//	获取玩家生命值（构造函数中也包含一部分）
//////////////////////////////////////////////////////////////////////////

//  该函数用以复制标有Replicated的任何属性，并且用以配置复制方式。
void ANetCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated CurrentHealth	声明血量值需要复制
	DOREPLIFETIME(ANetCharacter, CurrentHealth);
}

void ANetCharacter::OnHealthUpdate()
{
	return;
	//	判断客户端 （类似蓝图Switch Has Authority中的Romete引脚
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Blue,healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been Killed"));
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red, deathMessage);
		}
	}
	
	//	判断服务器 （类似Authority引脚
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining"),*GetFName().ToString(),CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	/*
		没有if判断的，就是所有机器都会执行的函数了。
		因任何伤害或死亡 而产生的特效功能 等 就可以放在之后
	*/
}

//  由于在GetLifetimeReplicatedProps中声明了Currenthealth需要Replicate，所以会调用OnRep_函数，所以OnHealthUpdate放在OnRep中，以改变就Update
void ANetCharacter::OnRep_CurrentHealth()
{
	if (CurrentHealth <= 0)
		DestroyDeath();
	OnHealthUpdate();
}

//////////////////////////////////////////////////////////////////////////
//	修改玩家生命值
//////////////////////////////////////////////////////////////////////////

// 血量值的更新，需要放在Server中，而不是Client。
void ANetCharacter::SetCurrentHealth(float healthValue)
{
	// 仅在服务器运行
	if (GetLocalRole() == ROLE_Authority)
	{
		//	0 ~ 100		由于CurrentHealth 更改，那么每个Client都会调用OnRep_()
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		// 调用OnHealthUpdate 确保Server和Client都对该函数 并行调用。
		OnHealthUpdate();
		//（由于Server不会收到RepNotify，因此需要在ROLE_Authority中调用OnHealthUpdate，确保Server和Client一致）
		if(CurrentHealth<=0)
			DestroyDeath();
	}

}

float ANetCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FinalDamageCauser = DamageCauser->GetOwner();
	
	//float damageApplied = CurrentHealth - DamageTaken;
	//SetCurrentHealth(damageApplied);
	//return damageApplied;
	return 0.f;
}

/// <summary>
/// 委托
/// </summary>
/// <param name="Data"></param>

void ANetCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	SetCurrentHealth(Health);
}


//////////////////////////////////////////////////////////////////////////
// 发射子弹逻辑
//////////////////////////////////////////////////////////////////////////


// 玩家本地调用，使用FireRate作为倒计时，调用StopFire，一起控制bIsFiringWeapon
void ANetCharacter::StartFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot)
{
	if (!bIsFiringWeapon)
	{
		
		// RPC函数必须控制调用频率，否则Client会掉线的可能
		if (BulletMaker == 0 || BulletMaker == 2)
		{
			bIsFiringWeapon = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &ANetCharacter::StopFire, FireRate, false);
			HandleFire(BulletMaker, factor, FVector(0.f, 0.f, 0.f), FRotator(0.f, 0.f, 0.f));
		}
		else if (BulletMaker == 1)
		{
			HandleFire(1, factor, cannonLoc, cannonRot);
		}
	}
}

void ANetCharacter::StopFire()
{
	bIsFiringWeapon = false;

}

// RPC的函数，末尾必须需要加上 "_Implementation"
void ANetCharacter::HandleFire_Implementation(int BulletMaker,float factor, FVector cannonLoc, FRotator cannonRot)
{
	
	FVector BulletLocation;
	FRotator BulletRotation;

	
	if (BulletMaker == 0 || BulletMaker == 2)
	{
		//	Player Fire
		
		// 获取摄像机变换
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// 设置发射物原点
		BulletOffset.Set(100.f, 0.f, 65.f);

		// 将BulletOffset从摄像机空间变换到世界空间
		BulletLocation = CameraLocation + FTransform(CameraRotation).TransformVector(BulletOffset) + FVector(0.f, 0.f, -50.f);

		// 是目标方向略向上倾斜
		BulletRotation = CameraRotation;
		BulletRotation.Pitch += 0.f;
		BulletRotation.Yaw += 1.f;


		FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.f) + (GetActorUpVector() * 50.f);
		FRotator spawnRotation = GetActorRotation();


	}
	else if (BulletMaker == 1)
	{
		// Cannon Fire
		BulletLocation = cannonLoc;
		BulletRotation = cannonRot;
	}
	

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ABulletProjectile* spawnedProjectile = GetWorld()->SpawnActor<ABulletProjectile>(ProjectileClass,BulletLocation,BulletRotation,spawnParameters);
	spawnedProjectile->factorValue = factor;
	if (BulletMaker == 2)
	{
		spawnedProjectile->RecoveryValue = RwIni->GetIniFloat(TEXT("PlayerAttr"),TEXT("Recovery"));
	}
	else {
		spawnedProjectile->RecoveryValue = 0;
	}
}

/// <summary>
/// 创造加农炮
/// </summary>

void ANetCharacter::MakeCannon()
{
	HandleCannon();
}

void ANetCharacter::HandleCannon_Implementation()
{
	// 以下注释的，用以生成 蓝图Actor，但是发布后会失效，只能在Editor使用。
	//UObject* SpawnActor = Cast<UObject>(
	//	StaticLoadObject(UObject::StaticClass(), NULL,
	//		TEXT("  '/Game/Leon/Cannon/BP_Cannon.BP_Cannon' ")));

	//UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
	//if (!SpawnActor)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CANT FIND OBJECT TO SPAWN")));
	//	return;
	//}

	//UClass* SpawnClass = SpawnActor->StaticClass();
	//if (SpawnClass == NULL)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
	//	return;
	//}

	// 以下可以成功。多一个步骤是，在Editor选择CannonActor
	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetActorLocation() + FVector(0.f, 0.f, -100.f), GetActorRotation(), SpawnParams);
	
	if (OldCannon == nullptr)
	{
		OldCannon = World->SpawnActor<AActor>(CannonActor, GetActorLocation() + GetActorForwardVector() * 100.f + FVector(150.f, 50.f, -100.f), GetActorRotation(), SpawnParams);
	}
	else
	{
		
		OldCannon->Destroy();
		OldCannon = World->SpawnActor<AActor>(CannonActor, GetActorLocation() + GetActorForwardVector() * 100.f + FVector(150.f, 50.f, -100.f), GetActorRotation(), SpawnParams);

	}

}

//////////////////////////////////////////////////////////////////////////
//	加农炮发射子弹的public函数，方便调用
//////////////////////////////////////////////////////////////////////////

void ANetCharacter::OtherFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot)
{
	StartFire(BulletMaker,factor,cannonLoc,cannonRot);
}



//////////////////////////////////////////////////////////////////////////
///	死亡后销毁
//////////////////////////////////////////////////////////////////////////

void ANetCharacter::DestroyDeath_Implementation()
{

	RemoveOldCannon();
	UE_LOG(LogTemp, Warning, TEXT("Owner is %s"), *FinalDamageCauser->GetFName().ToString());
	auto playerHub = Cast<ANetCharacter>(FinalDamageCauser);
	if (playerHub)
	{
		playerHub->AddKillCounts();
	}
	// 重生
	ANetGameModeBase* gamemode = GetWorld()->GetAuthGameMode<ANetGameModeBase>();
	if (gamemode)
	{
		gamemode->RequestRespawn(this,Controller);
	}
	
	// 最后销毁自身
	Destroy();
}

void ANetCharacter::RemoveOldCannon_Implementation()
{
	// 销毁自身炮台
	if (OldCannon != nullptr)
	{
		OldCannon->Destroy();
	}
}


void ANetCharacter::AddKillCounts_Implementation()
{
	KillCounts ++;
}


