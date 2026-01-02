

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyProject/BlasterTypes/TurningInPlace.h"
#include "MyProject/Interfaces/InteractWithCrosshairInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"


UCLASS()
class MYPROJECT_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	void UpdateHUDHealth();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	void Elim();
	UFUNCTION(NetMulticast,reliable)
	void MulticastElim();
	virtual void Destroyed() override;
	UFUNCTION(BlueprintCallable)
	void SpawnElimBot();
	
protected:
	
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	//poll for any relevant classes and initialize our HUD
	void PollInit();
	
	

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	
	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	
	UFUNCTION(Server, Reliable)
	void SeverEquipButtonPressed();
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	
	UPROPERTY(EditAnywhere, Category= Combat )
	class UAnimMontage* FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;
	
	void HideCameraIfCharacterClose();
	
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	/**
	 *Player Health
	 */
	
	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(ReplicatedUsing= OnRep_Health, VisibleAnywhere, Category="Player Stats")
	float Health = 100.f;
	
	UFUNCTION()
	void OnRep_Health();
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;
	
	bool bElimmed = false;
	
	FTimerHandle ElimTimerHandle;
	
	void ElimTimerFinished();
	
	UPROPERTY(EditdefaultsOnly)
	float ElimDelay = 4.f;
	
	/*
	 *dissolve effect
	 */
	
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	
	
	FOnTimelineFloat DissolveTrack;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;
	
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;
	
	
	//material instance set on the Blueprint,used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	
	//ElimMontage
	UPROPERTY()
	float ElimMontageLength = 0.f;
	
	FTimerHandle StartDissolveTimerHandle;
	
	//hide
	bool bPreHidden = false;
	
	UFUNCTION()
	void PreHideMesh(); 

	//ElimBot
	UPROPERTY(EditAnywhere, Category = ElimBot)
	UParticleSystem* ELimBotEffect;
	
	UPROPERTY(visibleAnywhere, Category = ElimBot)
	UParticleSystemComponent* ElimBotComponent;
	
	UPROPERTY(EditAnywhere, Category = ElimBot)
	class USoundCue* ElimBotSound;
	
	UPROPERTY()
	class ABlasteryPlayerState* BlasterPlayerState;
	
	/*
	//camera zooming in when aiming
	// 瞄准相机参数
	UPROPERTY(EditAnywhere, Category = Camera)
	float DefaultCameraArmLength = 600.f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float AimingCameraArmLength = 300.f; // 瞄准时拉近

	UPROPERTY(EditAnywhere, Category = Camera)
	FVector DefaultCameraSocketOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = Camera)
	FVector AimingCameraSocketOffset = FVector(0.f, 50.f, 20.f); // 往右、往上稍微偏一点

	UPROPERTY(EditAnywhere, Category = Camera)
	float DefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = Camera)
	float AimingFOV = 75.f; // 瞄准时稍微 zoom 一点

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraInterpSpeed = 10.f; // 插值速度
	*/
public:	
	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}
	
	
	FORCEINLINE bool IsElimmed() const {return bElimmed;}
	FORCEINLINE float GetHealth() const {return Health;}
	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	
	
};
