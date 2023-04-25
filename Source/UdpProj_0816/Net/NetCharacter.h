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

	//	Replicated Property	——属性复制
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

	//  存储最终击杀的人，方便计算人头分
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
/// 复活
/// </summary>
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	URwFiles* RwIni;

	// If init the type is FGameplayAttributeDate, this Value of Max value will be needn't.
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;	//	Cant Replicated, but need this for initializing Health. Cant change.

	// 声明CurrentHealth作为OnRep变量，并在下面自行创建一个RepNotify函数
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;		// Replicated, but cant read and change it.


	// This function will be called to tall all Clients when CurrentHealth change.
	UFUNCTION()
	void OnRep_CurrentHealth();

	//  响应要更新的CurrentHealth，修改后，立刻在服务器调用，并在客户端调用从而响应RepNotify
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay | Projectile")
	TSubclassOf<class ABulletProjectile> ProjectileClass;

	//	攻击频率（间隔
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float FireRate;

	bool bIsFiringWeapon;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartFire(int BulletMaker = 0, float factor = 1.f, FVector cannonLoc = FVector(0.f, 0.f, 0.f), FRotator cannonRot = FRotator(0.f, 0.f, 0.f));

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();

	// RPC 负责在Server生成发射物，在Server调用该函数，都会导致调用通过网络直接被重定向到服务器上的权威Role
	UFUNCTION(Server, Reliable)
	void HandleFire(int BulletMaker, float factor, FVector cannonLoc, FRotator cannonRot);

	// 限制player发射子弹频率的 Timer
	FTimerHandle FiringTimer;



	/// <summary>
	/// 召唤炮台Actor
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
/// 死亡后销毁
/// </summary>

	
	UFUNCTION(Server,Reliable)
	void DestroyDeath();


	/// <summary>
	/// 生命值改变的委托
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
