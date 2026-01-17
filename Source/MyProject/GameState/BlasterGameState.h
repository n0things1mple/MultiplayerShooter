// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScoringPlayers(class ABlasteryPlayerState* ScoringPlayer);
	UPROPERTY(Replicated)
	TArray<class ABlasteryPlayerState*> TopScoringPlayers;
private:
	float TopScore = 0.f;
};
