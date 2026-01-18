// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsCenter;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsLeft;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsRight;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsTop;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;

};


/**
 * 
 */
UCLASS()
class MYPROJECT_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	
	UPROPERTY(EditAnywhere, Category="Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	void AddCharacterOverlay();
	UPROPERTY(EditAnywhere, Category="Announcements")
	TSubclassOf<class UUserWidget> AnnouncementClass;
	UPROPERTY()
	class UAnnouncement* Announcement ;
	void AddAnnouncement();
	void ClearHUDPackage();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	virtual void BeginPlay() override;
	
	
private:
	UPROPERTY()
	FHUDPackage HUDPackage;
	
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);
	
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) {HUDPackage = Package;}
	
};
