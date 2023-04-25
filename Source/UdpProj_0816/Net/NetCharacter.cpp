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
	///	��ȡ�������ֵ
	//TODO: Replace using RwIni
	MaxHealth = RwIni->GetIniFloat(TEXT("PlayerAttr"),TEXT("MaxHealth"));
	CurrentHealth = MaxHealth;

	// ����
	ProjectileClass = ABulletProjectile::StaticClass();
	// ��ʼ������
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
	/// ע��ί��
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
//	��ȡ�������ֵ�����캯����Ҳ����һ���֣�
//////////////////////////////////////////////////////////////////////////

//  �ú������Ը��Ʊ���Replicated���κ����ԣ������������ø��Ʒ�ʽ��
void ANetCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicated CurrentHealth	����Ѫ��ֵ��Ҫ����
	DOREPLIFETIME(ANetCharacter, CurrentHealth);
}

void ANetCharacter::OnHealthUpdate()
{
	return;
	//	�жϿͻ��� ��������ͼSwitch Has Authority�е�Romete����
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
	
	//	�жϷ����� ������Authority����
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining"),*GetFName().ToString(),CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	/*
		û��if�жϵģ��������л�������ִ�еĺ����ˡ�
		���κ��˺������� ����������Ч���� �� �Ϳ��Է���֮��
	*/
}

//  ������GetLifetimeReplicatedProps��������Currenthealth��ҪReplicate�����Ի����OnRep_����������OnHealthUpdate����OnRep�У��Ըı��Update
void ANetCharacter::OnRep_CurrentHealth()
{
	if (CurrentHealth <= 0)
		DestroyDeath();
	OnHealthUpdate();
}

//////////////////////////////////////////////////////////////////////////
//	�޸��������ֵ
//////////////////////////////////////////////////////////////////////////

// Ѫ��ֵ�ĸ��£���Ҫ����Server�У�������Client��
void ANetCharacter::SetCurrentHealth(float healthValue)
{
	// ���ڷ���������
	if (GetLocalRole() == ROLE_Authority)
	{
		//	0 ~ 100		����CurrentHealth ���ģ���ôÿ��Client�������OnRep_()
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		// ����OnHealthUpdate ȷ��Server��Client���Ըú��� ���е��á�
		OnHealthUpdate();
		//������Server�����յ�RepNotify�������Ҫ��ROLE_Authority�е���OnHealthUpdate��ȷ��Server��Clientһ�£�
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
/// ί��
/// </summary>
/// <param name="Data"></param>

void ANetCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	SetCurrentHealth(Health);
}


//////////////////////////////////////////////////////////////////////////
// �����ӵ��߼�
//////////////////////////////////////////////////////////////////////////


// ��ұ��ص��ã�ʹ��FireRate��Ϊ����ʱ������StopFire��һ�����bIsFiringWeapon
void ANetCharacter::StartFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot)
{
	if (!bIsFiringWeapon)
	{
		
		// RPC����������Ƶ���Ƶ�ʣ�����Client����ߵĿ���
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

// RPC�ĺ�����ĩβ������Ҫ���� "_Implementation"
void ANetCharacter::HandleFire_Implementation(int BulletMaker,float factor, FVector cannonLoc, FRotator cannonRot)
{
	
	FVector BulletLocation;
	FRotator BulletRotation;

	
	if (BulletMaker == 0 || BulletMaker == 2)
	{
		//	Player Fire
		
		// ��ȡ������任
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// ���÷�����ԭ��
		BulletOffset.Set(100.f, 0.f, 65.f);

		// ��BulletOffset��������ռ�任������ռ�
		BulletLocation = CameraLocation + FTransform(CameraRotation).TransformVector(BulletOffset) + FVector(0.f, 0.f, -50.f);

		// ��Ŀ�귽����������б
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
/// �����ũ��
/// </summary>

void ANetCharacter::MakeCannon()
{
	HandleCannon();
}

void ANetCharacter::HandleCannon_Implementation()
{
	// ����ע�͵ģ��������� ��ͼActor�����Ƿ������ʧЧ��ֻ����Editorʹ�á�
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

	// ���¿��Գɹ�����һ�������ǣ���Editorѡ��CannonActor
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
//	��ũ�ڷ����ӵ���public�������������
//////////////////////////////////////////////////////////////////////////

void ANetCharacter::OtherFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot)
{
	StartFire(BulletMaker,factor,cannonLoc,cannonRot);
}



//////////////////////////////////////////////////////////////////////////
///	����������
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
	// ����
	ANetGameModeBase* gamemode = GetWorld()->GetAuthGameMode<ANetGameModeBase>();
	if (gamemode)
	{
		gamemode->RequestRespawn(this,Controller);
	}
	
	// �����������
	Destroy();
}

void ANetCharacter::RemoveOldCannon_Implementation()
{
	// ����������̨
	if (OldCannon != nullptr)
	{
		OldCannon->Destroy();
	}
}


void ANetCharacter::AddKillCounts_Implementation()
{
	KillCounts ++;
}


