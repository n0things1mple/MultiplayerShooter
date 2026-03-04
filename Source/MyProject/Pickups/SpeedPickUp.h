#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickUp.generated.h"

UCLASS()
class MYPROJECT_API ASpeedPickUp : public APickup
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
	float BaseSpeedBuff = 1000.f;

	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 600.f;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 5.f;
};