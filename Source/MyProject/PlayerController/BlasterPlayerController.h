// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void UpdateDeathMessage(const FString KilledByPlayerName);
	void HideDeathMessage();
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponIcon(UTexture2D* WeaponIconTexture);
	void SetHUDMatchCountdownText(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual float GetServerTime(); // synced with server world clock
	virtual void ReceivedPlayer() override; //sync with server clock asap
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	void SetHUDTime();

	
	//sync time between client and server
	
	//requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	
	//report the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest );
	
	float ClientServerDelta = 0.f; //difference between client and server time
	
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;
	
	float TimeSyncRunningTime = 0.f;
	
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	
	UFUNCTION( Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float WarmUp, float Match, float StartingTime);
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	
	uint32 CountDownInt = 0;
	
	UPROPERTY(ReplicatedUsing= OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;
	
};
