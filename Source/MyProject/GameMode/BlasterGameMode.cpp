// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "MyProject/PlayerState/BlasteryPlayerState.h"
#include "MyProject/PlayerController/BlasterPlayerController.h"

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,
                                        class ABlasterPlayerController* VictimController, class ABlasterPlayerController* AttackerController)
{
	ABlasteryPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasteryPlayerState>(AttackerController->PlayerState) : nullptr;
	
	ABlasteryPlayerState* VictimPlayerState = VictimController ? Cast<ABlasteryPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
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
