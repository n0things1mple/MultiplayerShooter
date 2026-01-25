#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class MYPROJECT_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileGrenade();

protected:
	virtual void BeginPlay() override;

	// 重写父类的OnHit，防止撞击时直接销毁
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	// 反弹时的逻辑
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	
	void Explode();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditAnywhere, Category = "Grenade")
	USoundBase* BounceSound;

	// 手榴弹引信时间（几秒后爆炸）
	UPROPERTY(EditAnywhere, Category = "Grenade")
	float FuseTime = 3.0f;

	// 定时器句柄
	FTimerHandle FuseTimerHandle;
};