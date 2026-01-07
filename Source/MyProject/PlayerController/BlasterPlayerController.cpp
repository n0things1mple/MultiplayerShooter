// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "MyProject/HUD/BlasterHUD.h"
#include "MyProject/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "Components/Image.h"



void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
	
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
	HideDeathMessage();
}


void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->HealthBar && 
				BlasterHUD->CharacterOverlay->HealthText;
	
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::RoundToInt(Health),FMath::RoundToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->ScoreAmount;
				
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"),FMath::RoundToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"),Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
}

void ABlasterPlayerController::UpdateDeathMessage(const FString KilledByPlayerName)
{
	if (KilledByPlayerName.IsEmpty())
	{
		HideDeathMessage();
		return;
	}
	
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->KilledBy && 
				BlasterHUD->CharacterOverlay->DeathMessage;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->KilledBy->SetText(FText::FromString(KilledByPlayerName));
		BlasterHUD->CharacterOverlay->DeathMessage->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->KilledBy->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterPlayerController::HideDeathMessage()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->KilledBy && 
				BlasterHUD->CharacterOverlay->DeathMessage;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->KilledBy->SetText(FText::FromString(""));
		BlasterHUD->CharacterOverlay->DeathMessage->SetVisibility(ESlateVisibility::Collapsed);
		BlasterHUD->CharacterOverlay->KilledBy->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"),Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"),Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDWeaponIcon(UTexture2D* WeaponIconTexture)
{
	if (!IsLocalController()) return;

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->WeaponIconImage;
	if (bHUDValid)
	{
		if (WeaponIconTexture)
		{
			BlasterHUD->CharacterOverlay->WeaponIconImage->SetBrushFromTexture(WeaponIconTexture);
			BlasterHUD->CharacterOverlay->WeaponIconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			BlasterHUD->CharacterOverlay->WeaponIconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

