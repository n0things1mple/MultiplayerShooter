// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"

#include "MyProject/Character/BlasterCharacter.h"

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,
                                        class ABlasterPlayerController* VictimController, class ABlasterPlayerController* AttackerController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}
