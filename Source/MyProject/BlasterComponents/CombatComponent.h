// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyProject/HUD/BlasterHUD.h"
#include "MyProject/Weapon/WeaponTypes.h"
#include "MyProject/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void EquipWeapon(class AWeapon* WeaponToEquip );
	void Reload();
	UFUNCTION(blueprintCallable)
	void FinishReloading();
	void FireButtionPressed(bool bPressed);
protected:
	
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	
	UFUNCTION(server, Reliable)
	void ServerSetAiming(bool bIsAiming);
	
	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();
	
	
	UFUNCTION( Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	
	
	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	
	void SetHUDCrosshairs(float DeltaTime);
	
	UFUNCTION(server, Reliable)
	void ServerReload();
	
	void HandleReload();
	
	int32 AmountToReload();
	
	void DropWeapon();
	UFUNCTION(server, Reliable)
	void ServerDropWeapon();
private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	UPROPERTY()
	class ABlasterHUD* HUD;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY( Replicated)
	bool bAiming;
	
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	
	bool bFireButtonPressed;
	
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* DefaultCrosshair;
	
	/**
	 * HUD and Crosshairs
	*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;	
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	FHUDPackage HUDPackage;
	
	FVector HitTarget;
	
	/**
	 *Aiming and FOV
	 */
	
	float DefaultFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	
	
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
	
	void InterpFOV(float DeltaTime);
	
	/*
	 *AutoFire
	 */
	FTimerHandle AutoFireTimer;
	
	
	
	bool bCanFire=true;
	
	void StartAutoFireTimer();
	void AutoFireTimerFinished();
	
	bool CanFire();
	
	//Carried ammo for the currently-equipped weapon type
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	UFUNCTION()
	void OnRep_CarriedAmmo();
	
	TMap<EWeaponType, int32> CarriedAmmoMap;
	
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;
	
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;
	
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 36;
	
	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo = 90;
	
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 12;
	
	void InitializeCarriedAmmo();
	
	UPROPERTY( ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	
	UFUNCTION()
	void OnRep_CombatState();
	
	void UpdateAmmoValues();
};



	
		

