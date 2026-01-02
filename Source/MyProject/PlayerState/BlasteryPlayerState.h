// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasteryPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABlasteryPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();
	UFUNCTION()
	virtual void OnRep_KilledBy();
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	void UpdateDeathMessage(FString KilledByPlayerName);
private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	UPROPERTY()
	class ABlasterPlayerController* Controller;
	
	UPROPERTY(ReplicatedUsing= OnRep_Defeats)
	int32 Defeats;
	
	UPROPERTY(ReplicatedUsing= OnRep_KilledBy)
	FString KilledBy;
	
	
};
