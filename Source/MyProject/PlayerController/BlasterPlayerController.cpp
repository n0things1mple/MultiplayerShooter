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
#include "MyProject/HUD/Announcement.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MyProject/BlasterComponents/CombatComponent.h"
#include "MyProject/GameState/BlasterGameState.h"
#include "MyProject/PlayerState/BlasteryPlayerState.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	
	bReceivedMatchInfo = false;
	MatchInfoRetryTime = 0.f;
	CountDownInt = -1;

	ServerCheckMatchState();
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}


void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryInitHUD();
	SetHUDTime();
	
	if (!bReceivedMatchInfo)
	{
		MatchInfoRetryTime += DeltaTime;
		if (MatchInfoRetryTime >= 0.25f)
		{
			ServerCheckMatchState();
			MatchInfoRetryTime = 0.f;
		}
	}
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	
	
	if (bInitializeCharacterOverlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tick: Polling for HUD Init..."));
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
			if (BlasterCharacter)
			{
				SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
				SetHUDScore(0);
				SetHUDDefeats(0);
			}
		}
		
	}

	
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
		bInitializeCharacterOverlay = true;
		SetHUDHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
	SetHUDCarriedAmmo(0); 
	SetHUDWeaponIcon(nullptr);
	HideDeathMessage();
}

void ABlasterPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	if (IsLocalController())
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
		if (BlasterCharacter)
		{
			// 【新增】显式开启轮询
			bInitializeCharacterOverlay = true; 

			// 尝试立即设置
			SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		}
	}
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
		UE_LOG(LogTemp, Warning, TEXT("SetHUDHealth: SUCCESS! HUD Name: %s"), *BlasterHUD->GetName());
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::RoundToInt(Health),FMath::RoundToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
		bInitializeCharacterOverlay = false;
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

void ABlasterPlayerController::SetHUDMatchCountdownText(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && 
		BlasterHUD->Announcement &&
			BlasterHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()) : BlasterGameMode;
		if (BlasterGameMode)
		{
			LevelStartingTime = BlasterGameMode->LevelStartingTime;
		}
	}
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - (GetServerTime() - LevelStartingTime);
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = MatchTime - (GetServerTime() - LevelStartingTime - WarmupTime);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime - (GetServerTime() - LevelStartingTime - WarmupTime - MatchTime);
	}
	
	uint32 SecondLeft = FMath::CeilToInt(TimeLeft);
	if (CountDownInt != SecondLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdownText(TimeLeft);
		}
	}
	CountDownInt = SecondLeft;
}

void ABlasterPlayerController::TryInitHUD()
{
	if (!BlasterHUD)
	{
		BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	}
	if (!BlasterHUD) return;

	// 确保对应状态的widget一定存在
	if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
	{
		BlasterHUD->AddAnnouncement();
	}
	else if (MatchState == MatchState::InProgress)
	{
		BlasterHUD->AddCharacterOverlay();
	}
}


void ABlasterPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ClearAllAnnouncementWidgets();
	ClearAllCharacterOverlayWidgets();
	
	BlasterHUD = nullptr;
	bInitializeCharacterOverlay = true;

	bReceivedMatchInfo = false;
	MatchInfoRetryTime = 0.f;
	CountDownInt = -1;

	ServerCheckMatchState();
	TryInitHUD();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
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
	TryInitHUD(); // 先确保 UI 存在和显示正确
	
	if (MatchState == MatchState::WaitingToStart)
	{
		HandleWaitingToStart();
	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


void ABlasterPlayerController::OnRep_MatchState()
{
	TryInitHUD();
	
	if (MatchState == MatchState::WaitingToStart)
	{
		HandleWaitingToStart();
	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;
	

	WarmupTime = GameMode->WarmupTime;
	MatchTime = GameMode->MatchTime;
	CooldownTime = GameMode->CooldownTime;
	LevelStartingTime = GameMode->LevelStartingTime;
	MatchState = GameMode->GetMatchState();

	ClientJoinMidgame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
}

void ABlasterPlayerController::ClearAllAnnouncementWidgets()
{
	if (!IsLocalController()) return;

	TArray<UUserWidget*> Found;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Found, UAnnouncement::StaticClass(), false);
	for (UUserWidget* W : Found)
	{
		if (W)
		{
			W->RemoveFromParent();
		}
	}
}

void ABlasterPlayerController::ClearAllCharacterOverlayWidgets()
{
	if (!IsLocalController()) return;

	TArray<UUserWidget*> Found;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, Found, UCharacterOverlay::StaticClass(), false);
	for (UUserWidget* W : Found)
	{
		if (W)
		{
			W->RemoveFromParent();
		}
	}
}

void ABlasterPlayerController::HandleWaitingToStart()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (!BlasterHUD) return;

	BlasterHUD->AddAnnouncement();

	if (BlasterHUD->CharacterOverlay)
	{
		BlasterHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	if (BlasterHUD->Announcement)
	{
		BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
	}
	if (MatchState == MatchState::WaitingToStart && IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}

	CountDownInt = -1;
	SetHUDTime();
}


void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float WarmUp, float Match, float StartingTime, float Cooldown)
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT: ClientJoinMidgame. PC=%s State=%s Warmup=%.2f Match=%.2f Cooldown=%.2f Start=%.2f"),
		*GetName(), *StateOfMatch.ToString(), WarmUp, Match, Cooldown, StartingTime);

	WarmupTime = WarmUp;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;

	bReceivedMatchInfo = true;
	MatchInfoRetryTime = 0.f;
	CountDownInt = -1;

	TryInitHUD();
	OnMatchStateSet(StateOfMatch);
	
	SetHUDTime();
}



void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility( ESlateVisibility::Hidden);
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->ClearHUDPackage();
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->RemoveFromParent();
		}
		bool bHUDValid = BlasterHUD->Announcement && 
			BlasterHUD->Announcement->AnnouncementText && 
				BlasterHUD->Announcement->InfoText;
		
		if (bHUDValid)
		{
			BlasterHUD->Announcement->SetVisibility( ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());
			ABlasteryPlayerState* BlasterPlayerState = GetPlayerState<ABlasteryPlayerState>();
			if (BlasterGameState)
			{
				TArray<ABlasteryPlayerState*>TopScoringPlayer = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopScoringPlayer.Num() == 0)
				{
					InfoTextString = FString("There is no winner this round.");
				}
				else if (TopScoringPlayer.Num() == 1 && TopScoringPlayer[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You Are The Winner!");
				}
				else if (TopScoringPlayer.Num() == 1 && TopScoringPlayer[0] != BlasterPlayerState)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopScoringPlayer[0]->GetPlayerName());
				}
				else if (TopScoringPlayer.Num() > 1)
				{
					InfoTextString = FString("Players Tied For The Win: \n");
					for (auto TiedPlayer : TopScoringPlayer)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter &&BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombatComponent()->FireButtionPressed(false);
	}
	
}
