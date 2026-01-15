// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyProject/Weapon/Weapon.h"
#include "MyProject/BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = BlasterCharacter ->GetCharacterMovement()->IsFalling();
	
	BIsAccelerating = BlasterCharacter ->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	
	bIsCrouched = BlasterCharacter->bIsCrouched;
	
	bAiming = BlasterCharacter->IsAiming();
	
	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	
	bElimmed = BlasterCharacter->IsElimmed();
	

	
	//Offset Yaw For Strafing
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 2.f);
	YawOffset = DeltaRotation.Yaw;
	
	CharacterRotationLastFrame = CharacterRotationThisFrame;
	CharacterRotationThisFrame = BlasterCharacter->GetActorRotation();
	const FRotator Delta =UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotationThisFrame, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float NewLean = FMath::FInterpTo(Lean, Target, DeltaSeconds, 2.f);
	Lean = FMath::Clamp(NewLean, -90.f, 90.f);
	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket", ERelativeTransformSpace::RTS_World));
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("Hand_R"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition,OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
		
		if (BlasterCharacter)
		{
			// 每帧都更新，别只在 if 里设 true
			bLocallyControlled = BlasterCharacter->IsLocallyControlled();

			const FTransform RightHandTransform =
				BlasterCharacter->GetMesh()->GetSocketTransform(FName("Hand_R"), RTS_World);

			const FVector HandLoc = RightHandTransform.GetLocation();

			FVector TargetPoint;

			if (bLocallyControlled)
			{
				// 本地：用准星 trace 的命中点（最准）
				TargetPoint = BlasterCharacter->GetHitTarget();
			}
			else
			{
				// 远端：用角色的 BaseAimRotation（会包含 RemoteViewPitch 等，适用于模拟代理）
				const FRotator AimRot = BlasterCharacter->GetBaseAimRotation();
				TargetPoint = HandLoc + AimRot.Vector() * 10000.f;   // 乘 1000/10000 都行，远一点更稳定
			}

			// 你原来的“镜像目标点”（反方向补偿）逻辑保持不变
			const FVector MirroredTarget = HandLoc + (HandLoc - TargetPoint);

		
			FRotator DesiredRot = UKismetMathLibrary::FindLookAtRotation(HandLoc, MirroredTarget);
			DesiredRot.Roll += 180.f;

			RightHandRotation = FMath::RInterpTo(RightHandRotation, DesiredRot, DeltaSeconds, 30.f);
			RightHandRotation.Normalize();

		}

	}
	bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
	bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading && !BlasterCharacter->GetDisableGameplay();
	
}