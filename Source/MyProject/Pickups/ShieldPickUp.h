#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickUp.generated.h"

UCLASS()
class MYPROJECT_API AShieldPickUp : public APickup
{
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere)
	float ShieldReplenishAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float ShieldReplenishTime = 5.f;
};