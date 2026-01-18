// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "Announcement.h"
#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"



void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC || !CharacterOverlayClass) return;

	if (!CharacterOverlay)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PC, CharacterOverlayClass);
	}
	if (CharacterOverlay && !CharacterOverlay->IsInViewport())
	{
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController || !AnnouncementClass) return;
	if (!Announcement)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
	}
	if (Announcement && !Announcement->IsInViewport())
	{
		Announcement->AddToViewport();
	}
	else if (Announcement)
	{
		Announcement->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void ABlasterHUD::ClearHUDPackage()
{
	HUDPackage = FHUDPackage();
}

void ABlasterHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->RemoveFromParent();
		CharacterOverlay = nullptr;
	}
	if (Announcement)
	{
		Announcement->RemoveFromParent();
		Announcement = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}


void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X/2.f,ViewportSize.Y/2.f);
		
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f,0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread,HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter,Spread,HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter,Spread,HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f,-SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter,Spread,HUDPackage.CrosshairColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter,Spread,HUDPackage.CrosshairColor);
		}
	}
}



void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	if (!Texture || !IsValid(Texture)) return;        
	if (!Texture->GetResource()) return;                
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(ViewportCenter.X -TextureWidth/2.f +Spread.X,ViewportCenter.Y-TextureHeight/2.f + Spread.Y);
	
	DrawTexture(Texture,TextureDrawPoint.X,TextureDrawPoint.Y,TextureWidth,TextureHeight,0.f,0.f,1.f,1.f,CrosshairColor);
}
