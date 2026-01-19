// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"

AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	AController* FiringController = GetInstigatorController();
	if (FiringController)
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this, //World context object
			Damage,
			10.f,
			GetActorLocation(),
			200.f,
			500.f,
			1.f,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			this,
			FiringController);
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
