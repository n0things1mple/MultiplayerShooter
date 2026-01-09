// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "MyProject/HUD/BlasterHUD.h"
#include "MyProject/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyProject/Character/BlasterCharacter.h"
#include "Components/Image.h"
#include "Net//UnrealNetwork.h"
#include "MyProject/GameMode/BlasterGameMode.h"



void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
}
void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}


void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetHUDTime();
	
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
	PollInit();
	
}

float ABlasterPlayerController::GetServerTime()
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}




void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
	SetHUDCarriedAmmo(0); 
	SetHUDWeaponIcon(nullptr);
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
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

void ABlasterPlayerController::SetHUDMatchCountdownText(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	uint32 SecondLeft = FMath::CeilToInt(MatchTime - GetServerTime());
	if (CountDownInt != SecondLeft)
	{
		SetHUDMatchCountdownText(MatchTime - GetServerTime());
	}
	CountDownInt = SecondLeft;
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth,HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
				bInitializeCharacterOverlay = false;
			}
		}
	}
}


void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest,ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5 * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
	
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			BlasterHUD->AddCharacterOverlay();
		}
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			BlasterHUD->AddCharacterOverlay();
		}
	}
}