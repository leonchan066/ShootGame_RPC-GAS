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


	// ֪ͨUE����Actor��ҪbReplicates		Ĭ�����ֻ����Owning Client����Ϊture��ֻҪActor��Authority Actor����Server���ͻᱻ���Ƶ�����Cilent
	bReplicates = true;


	// ���巢�����Լ�SphereCollision
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(25.f);
	SphereComponent->SetCollisionProfileName(TEXT("Bullet"));
	//SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;
	// ��Server�ϵ�OnComponentHit��ע��OnProjectileImpact
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this,&ABulletProjectile::OnProjectileImpact);
	}



	// ����Objģ��
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT(" '/Game/Leon/Gun/Bullet.Bullet'"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// ���ɹ��ҵ�Obj��Դ�����ø�ģ�͵�λ�������
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.f));
		StaticMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	}

	/// �����ЧEffect

	// ���Projectile
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	float speed = 1000.f * 25;
	ProjectileMovementComponent->InitialSpeed = speed;
	ProjectileMovementComponent->MaxSpeed = speed;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	// �ӵ����˺�������
	DamageType = UDamageType::StaticClass();
	DamageValue = 10.f;
	// Ĭ�ϼ�ѪΪ�㡣��Ҫʹ��RecoveryFire���ü�Ѫ��
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


// �����ٺ����
void ABulletProjectile::Destroyed()
{
	//�����ٵ�λ�ò���Effect		�Ӷ���ÿ��Cilent��������ըЧ��
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
			//	�˺�С��20����Ѫ�˺�
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
			// �˺����ڻ����20��˲���˺�
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




	Destroy();		//ֻҪײ����Ҫ���٣�һ�����پͻ����Destroyed����Effect
}

