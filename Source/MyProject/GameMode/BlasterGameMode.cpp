// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Character/BlasterCharacter.h"

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,
                                        class ABlasterPlayerController* VictimController, class ABlasterPlayerController* AttackerController)
{
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
	}
}
