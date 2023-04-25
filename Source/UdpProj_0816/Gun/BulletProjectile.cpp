// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "UdpProj_0816/Gas/NetAttributeSet.h"
#include "UdpProj_0816/Net/NetCharacter.h"
#include "UdpProj_0816/Gun/Cannon.h"


// Sets default values
ABulletProjectile::ABulletProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// 通知UE，该Actor需要bReplicates		默认情况只能在Owning Client，设为ture后，只要Actor的Authority Actor存在Server，就会被复制到各个Cilent
	bReplicates = true;


	// 定义发射物以及SphereCollision
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(25.f);
	SphereComponent->SetCollisionProfileName(TEXT("Bullet"));
	//SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;
	// 与Server上的OnComponentHit上注册OnProjectileImpact
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this,&ABulletProjectile::OnProjectileImpact);
	}



	// 定义Obj模型
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT(" '/Game/Leon/Gun/Bullet.Bullet'"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// 若成功找到Obj资源，设置该模型的位置与比例
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.f));
		StaticMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	}

	/// 添加特效Effect

	// 添加Projectile
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	float speed = 1000.f * 25;
	ProjectileMovementComponent->InitialSpeed = speed;
	ProjectileMovementComponent->MaxSpeed = speed;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	// 子弹的伤害与属性
	DamageType = UDamageType::StaticClass();
	DamageValue = 10.f;
	// 默认加血为零。需要使用RecoveryFire调用加血。
	RecoveryValue = 0;

	InitialLifeSpan = 3.f;
}



// Called when the game starts or when spawned
void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// 被销毁后调用
void ABulletProjectile::Destroyed()
{
	//在销毁的位置播放Effect		从而让每个Cilent都看到爆炸效果
	FVector SpawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, SpawnLocation,FRotator::ZeroRotator,true,EPSCPoolMethod::AutoRelease);

}


void ABulletProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	auto HubCannon = Cast<ACannon>(OtherActor);
	if (HubCannon != nullptr)
	{			
		UE_LOG(LogTemp,Warning,TEXT("%s"),*HubCannon->GetFName().ToString());
		UE_LOG(LogTemp,Warning,TEXT("%s"),*GetOwner()->GetFName().ToString());
		
		if (HubCannon->GetOwner() == GetOwner())
		{

			float UpCannonHub = RwIni->GetIniFloat(TEXT("PlayerAttr"), TEXT("BaseCannonLevel"));
			UGameplayStatics::ApplyPointDamage(OtherActor, UpCannonHub, NormalImpulse, Hit, GetInstigatorController(), this, DamageType);

			if (RecoveryValue != 0) 
			{
				RecoveryValue = FMath::Clamp(RecoveryValue, 1.f, 20.f);
				HubCannon->SetCurrentHealth(HubCannon->GetCurrentHealth() + RecoveryValue * factorValue);
			}


		}
		

	}
	auto HubEnemy = Cast<ANetCharacter>(OtherActor);
	if (HubEnemy != nullptr )
	{
		float DamageHub = RwIni->GetIniFloat(TEXT("PlayerAttr"), TEXT("BaseAck"));

		if ( DamageHub * factorValue <= 20) 
		{
			//	伤害小于20，流血伤害
			UAbilitySystemComponent* TargetASC = HubEnemy->leonASC;
			UNetAttributeSet* TargetAttr = HubEnemy->NetAttrSetComp;

			UGameplayEffect* BulletDamage = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("BulletDamage")));
			/* Instant */
			//BulletDamage->DurationPolicy = EGameplayEffectDurationType::Instant;
			/**/

			/* HasDuratoin */
			BulletDamage->DurationPolicy = EGameplayEffectDurationType::HasDuration;

			BulletDamage->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.f));
			BulletDamage->Period.Value = 1.5f;
			/**/

			int32 Idx = BulletDamage->Modifiers.Num();
			BulletDamage->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoHealth = BulletDamage->Modifiers[Idx];
			
			//float hubDamage = RwIni->GetIniFloat(TEXT("PlayerAttr"), TEXT("BaseAck"));
			
			InfoHealth.ModifierMagnitude = FScalableFloat( -DamageHub * factorValue);
			InfoHealth.ModifierOp = EGameplayModOp::Additive;
			InfoHealth.Attribute = UNetAttributeSet::GetHealthAttribute();

			//HubEnemy.leonASC->ApplyGameplayEffectToSelf(BulletDamage, 1.f, HubEnemy.leonASC->MakeEffectContext());
			TargetASC->ApplyGameplayEffectToSelf(BulletDamage, 1.f, TargetASC->MakeEffectContext());

			UE_LOG(LogTemp, Warning, TEXT("Health : %f"), TargetAttr->GetHealth().GetCurrentValue());
			UE_LOG(LogTemp, Warning, TEXT("Health : %s"), *TargetASC->GetFName().ToString());
		}
		else
		{
			// 伤害大于或等于20，瞬间伤害
			UAbilitySystemComponent* TargetASC = HubEnemy->leonASC;
			UNetAttributeSet* TargetAttr = HubEnemy->NetAttrSetComp;

			UGameplayEffect* BulletDamage = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("BulletDamage")));
			BulletDamage->DurationPolicy = EGameplayEffectDurationType::Instant;
			
			int32 Idx = BulletDamage->Modifiers.Num();
			BulletDamage->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoHealth = BulletDamage->Modifiers[Idx];
			InfoHealth.ModifierMagnitude = FScalableFloat(-DamageHub * factorValue);
			InfoHealth.ModifierOp = EGameplayModOp::Additive;
			InfoHealth.Attribute = UNetAttributeSet::GetHealthAttribute();

			//HubEnemy.leonASC->ApplyGameplayEffectToSelf(BulletDamage, 1.f, HubEnemy.leonASC->MakeEffectContext());
			TargetASC->ApplyGameplayEffectToSelf(BulletDamage, 1.f, TargetASC->MakeEffectContext());

			UE_LOG(LogTemp, Warning, TEXT("Health : %f"), TargetAttr->GetHealth().GetCurrentValue());
			UE_LOG(LogTemp, Warning, TEXT("Health : %s"), *TargetASC->GetFName().ToString());
		}

		HubEnemy->HitByBulletCpp();
		
	}



		
	if (OtherActor)
	{
		float DamageHub = RwIni->GetIniFloat(TEXT("PlayerAttr"), TEXT("BaseAck"));
		UGameplayStatics::ApplyPointDamage(OtherActor, DamageHub * factorValue, NormalImpulse, Hit, GetInstigatorController(), this, DamageType);
	}




	Destroy();		//只要撞到就要销毁，一旦销毁就会调用Destroyed播放Effect
}

