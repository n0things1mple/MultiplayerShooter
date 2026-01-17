// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "MyProject/PlayerState/BlasteryPlayerState.h"
#include "MyProject/PlayerController/BlasterPlayerController.h"
#include "myproject/GameState/BlasterGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}



void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState( MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}



void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,
                                        class ABlasterPlayerController* VictimController, class ABlasterPlayerController* AttackerController)
{
	ABlasteryPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasteryPlayerState>(AttackerController->PlayerState) : nullptr;
	
	ABlasteryPlayerState* VictimPlayerState = VictimController ? Cast<ABlasteryPlayerState>(VictimController->PlayerState) : nullptr;
	
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScoringPlayers(AttackerPlayerState);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if (AttackerPlayerState && VictimPlayerState)
	{
		VictimPlayerState->UpdateDeathMessage(AttackerPlayerState->GetPlayerName());
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedCharacter)
	{
		TArray<AActor*> ActorStarts;
		UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),ActorStarts);
		int32 Selection = FMath::RandRange(0,ActorStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController,ActorStarts[Selection]);
		
		ABlasteryPlayerState* BlasterPlayerState = ElimmedController ? Cast<ABlasteryPlayerState>(ElimmedController->PlayerState) : nullptr;
		if (BlasterPlayerState)
		{
			BlasterPlayerState->UpdateDeathMessage(FString(""));
		}
	}
}


