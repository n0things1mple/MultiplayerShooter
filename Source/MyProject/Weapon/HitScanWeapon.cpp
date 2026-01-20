// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget-Start) * 1.25f;
		
		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(FireHit, Start, End, ECC_Visibility);
		}
		FVector BeamEnd = End;
		if (FireHit.bBlockingHit)
		{
			BeamEnd = FireHit.ImpactPoint;
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				if (HasAuthority())
				{
					UGameplayStatics::ApplyDamage(BlasterCharacter, 
						Damage, 
						GetInstigatorController(), 
						this, 
						UDamageType::StaticClass());
				}
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), 
					ImpactParticles, 
					FireHit.ImpactPoint, 
					FireHit.ImpactNormal.Rotation());
			}
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), 
				BeamParticles, 
				SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
