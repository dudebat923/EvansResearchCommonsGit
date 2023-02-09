
#include "PreswerxSiteSwitcher.h"
#include "Kismet/GameplayStatics.h"

// Sets default values.
APreswerxSiteSwitcher::APreswerxSiteSwitcher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned.
void APreswerxSiteSwitcher::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::LoadStreamLevel(this, SiteVariations[CurrentSiteIndex], true, false, LatentInfo);
}

// Called every frame (Tick is disabled for this class).
void APreswerxSiteSwitcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Replaces the current sub-level with the next one in the SiteVariations array.
void APreswerxSiteSwitcher::SwitchToNextSite()
{
	const int32 NextIndex = CurrentSiteIndex + 1;

	if (NextIndex + 1 <= SiteVariations.Num())
	{
		ChangeLoadedLevel(NextIndex);
		CurrentSiteIndex++;
	}
	else
	{
		ChangeLoadedLevel(0);
		CurrentSiteIndex = 0;
	}
}

// Replaces the current sub-level with the previous one in the SiteVariations array.
void APreswerxSiteSwitcher::SwitchToPreviousSite()
{
	const int32 PreviousIndex = CurrentSiteIndex - 1;

	if (PreviousIndex >= 0)
	{
		ChangeLoadedLevel(PreviousIndex);
		CurrentSiteIndex--;
	}
	else
	{
		ChangeLoadedLevel(SiteVariations.Num()-1);
		CurrentSiteIndex = SiteVariations.Num() - 1;
	}
}

// Loads the desired sub-level.
void APreswerxSiteSwitcher::ChangeLoadedLevel(int32 NewIndex)
{
	UGameplayStatics::LoadStreamLevel(this, SiteVariations[NewIndex], true, false, LatentInfo);
	TimerDelegate.BindUFunction(this, FName("UnloadLastSite"), CurrentSiteIndex);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0, false, 0.01);
}

// Unloads the last sub-level.
void APreswerxSiteSwitcher::UnloadLastSite(int32 SiteIndex)
{
	UGameplayStatics::UnloadStreamLevel(this, SiteVariations[SiteIndex], LatentInfo, false);
}

