#include "ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProjectileGrenade::AProjectileGrenade()
{
	
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.6f; // 弹性 (0-1)
	ProjectileMovementComponent->Friction = 0.2f;   // 摩擦力
}

void AProjectileGrenade::BeginPlay()
{
	Super::BeginPlay(); 

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);

	
	GetWorldTimerManager().SetTimer(
		FuseTimerHandle, 
		this, 
		&AProjectileGrenade::Explode, 
		FuseTime, 
		false
	);
}


void AProjectileGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}

void AProjectileGrenade::Explode()
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
	Destroy();
}