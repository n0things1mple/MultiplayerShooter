// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasteryPlayerState.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "MyProject/PlayerController/BlasterPlayerController.h"

void ABlasteryPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
	
}

void ABlasteryPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}
