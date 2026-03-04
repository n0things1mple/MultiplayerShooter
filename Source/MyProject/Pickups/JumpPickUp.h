#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickUp.generated.h"

UCLASS()
class MYPROJECT_API AJumpPickUp : public APickup
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
	float JumpZVelocityBuff = 3000.f;

	UPROPERTY(EditAnywhere)
	float JumpBuffTime = 10.f;
};