// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "UdpProj_0816/Gas/NetAttributeSet.h"
#include "CoreMinimal.h"
#include "UdpProj_0816/RwFiles.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "NetCharacter.generated.h"

UCLASS()
class UDPPROJ_0816_API ANetCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetCharacter();

	//	Replicated Property	�������Ը���
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GameplayAbilities, meta = (AllowPrivateAccess = "true"))
		class UAbilitySystemComponent* AbilitySystem;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<class UGameplayAbility>> GameplayAbilities;

	UPROPERTY(BlueprintReadOnly)
		UAbilitySystemComponent* leonASC;

/// <summary>
/// GAS
/// </summary>

	//UFUNCTION(BlueprintCallable , Category = "Player | UGameplayAbilities")
	//void AquireAbility(TSubclassOf<UGameplayAbility> listAbilites);

/// <summary>
/// AttrSet
/// </summary>

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetPlayerAttrs")
	class UNetAttributeSet* NetAttrSetComp;

	//  �洢���ջ�ɱ���ˣ����������ͷ��
	UPROPERTY()
	AActor* FinalDamageCauser;

	UPROPERTY(BlueprintReadWrite)
	int32 KillCounts;

	UFUNCTION(Client,Reliable)
	void AddKillCounts();

public:
	// Get the Max Health Value
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	//	Get the Current Health Value
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const {return CurrentHealth;}
	
	//	The scope of Health is 0 ~ 100, Call OnHealthUpdate, only on Server
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);

	// Taken the Damage, override from Pawn
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// The origin point making Bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector BulletOffset;

	UFUNCTION(BlueprintImplementableEvent)
	void HitByBulletCpp();

/// <summary>
/// Cannnon Fire Public Function
/// </summary>

	UFUNCTION(BlueprintCallable)
	void OtherFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> CannonActor;

/// <summary>
/// ����
/// </summary>
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	URwFiles* RwIni;

	// If init the type is FGameplayAttributeDate, this Value of Max value will be needn't.
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;	//	Cant Replicated, but need this for initializing Health. Cant change.

	// ����CurrentHealth��ΪOnRep�����������������д���һ��RepNotify����
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;		// Replicated, but cant read and change it.


	// This function will be called to tall all Clients when CurrentHealth change.
	UFUNCTION()
	void OnRep_CurrentHealth();

	//  ��ӦҪ���µ�CurrentHealth���޸ĺ������ڷ��������ã����ڿͻ��˵��ôӶ���ӦRepNotify
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay | Projectile")
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	//	����Ƶ�ʣ����
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float FireRate;

	bool bIsFiringWeapon;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartFire(int BulletMaker = 0, float factor = 1.f, FVector cannonLoc = FVector(0.f, 0.f, 0.f), FRotator cannonRot = FRotator(0.f, 0.f, 0.f));

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();

	// RPC ������Server���ɷ������Server���øú��������ᵼ�µ���ͨ������ֱ�ӱ��ض��򵽷������ϵ�Ȩ��Role
	UFUNCTION(Server, Reliable)
	void HandleFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot);

	// ����player�����ӵ�Ƶ�ʵ� Timer
	FTimerHandle FiringTimer;



	/// <summary>
	/// �ٻ���̨Actor
	/// </summary>

	UFUNCTION(BlueprintCallable)
		void MakeCannon();

	UFUNCTION(Server, Reliable)
		void HandleCannon();

	UPROPERTY()
	AActor* OldCannon;

	UFUNCTION(Server, Reliable , BlueprintCallable)
	void RemoveOldCannon();

/// <summary>
/// ����������
/// </summary>

	
	UFUNCTION(Server,Reliable)
	void DestroyDeath();


	/// <summary>
	/// ����ֵ�ı��ί��
	/// </summary>

	FDelegateHandle HealthChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	



};
