// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include  "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MYPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	
	/**
	 * Textures for the weapon crosshairs
	 */
	UPROPERTY(EditAnywhere, Category= Crosshairs)
	class UTexture2D* CrosshairCenter;
	
	UPROPERTY(EditAnywhere, Category= Crosshairs)
	class UTexture2D* CrosshairLeft;
	
	UPROPERTY(EditAnywhere, Category= Crosshairs)
	class UTexture2D* CrosshairRight;
	
	UPROPERTY(EditAnywhere, Category= Crosshairs)
	class UTexture2D* CrosshairTop;
	
	UPROPERTY(EditAnywhere, Category= Crosshairs)
	class UTexture2D* CrosshairBottom;
	
	/**
	*Zoomed FOV while aiming
	*/
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;
	
	/*
	 *Automatic Fire
	 */
	UPROPERTY(EditAnywhere, Category = Combat)
	float AutoFireDelay=0.15f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
	
	//Weapon Icon
	UPROPERTY(EditAnywhere)
	UTexture2D* WeaponIconTexture;
	
	//weapon attach righthand transform
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Attach")
	FName HandSocketName = FName("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category="Weapon|Attach")
	FTransform HandSocketOffset; 


protected:
	virtual void BeginPlay() override;
	
	
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);
	

private:
	UPROPERTY(VisibleAnywhere, category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, category = "Weapon Properties")
	class USphereComponent* AreaSphere;
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere, category = "Weapon Properties")
	EWeaponState WeaponState;
	
	UPROPERTY( VisibleAnywhere, category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;
	
	UFUNCTION()
	void OnRep_WeaponState();
	
	UPROPERTY(editanywhere, Category="Weapon Properties")
	class UAnimationAsset* FireAnimation;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
	
	UPROPERTY(EditAnywhere, ReplicatedUsing= OnRep_Ammo)
	int32 Ammo;
	
	UFUNCTION()
	void OnRep_Ammo();
	
	void SpendRound();
	
	UPROPERTY(EditAnywhere)
	int32 MaxAmmo;
	
	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;
	
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
public:	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const {return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	FORCEINLINE float GetZoomFOV() const {return ZoomFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	bool IsEmpty();
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType; }
	FORCEINLINE int32 GetAmmo() const {return Ammo;}
	FORCEINLINE int GetMaxAmmo() const {return MaxAmmo;}
	FORCEINLINE UTexture2D* GetWeaponHUD() const{ return WeaponIconTexture; }
};
