
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Engine/LatentActionManager.h"
#include "PreswerxSiteSwitcher.generated.h"

UCLASS()
class PRESWERXSWITCHER_API APreswerxSiteSwitcher : public AActor
{
	GENERATED_BODY()

// FUNCTIONS
public:	
	// Sets default values for this actor's properties.
	APreswerxSiteSwitcher();

protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

public:	
	// Called every frame (Tick is disabled for this class).
	virtual void Tick(float DeltaTime) override;

protected:
	// Replaces the current sub-level with the next one in the SiteVariations array.
	UFUNCTION(BlueprintCallable, Category = "PreswerxSiteSwitcher")
	void SwitchToNextSite();

	// Replaces the current sub-level with the previous one in the SiteVariations array.
	UFUNCTION(BlueprintCallable, Category = "PreswerxSiteSwitcher")
	void SwitchToPreviousSite();

	// Loads the desired sub-level.
	UFUNCTION()
	void ChangeLoadedLevel(int32 NewIndex);

	// Unloads the last sub-level.
	UFUNCTION()
	void UnloadLastSite(int32 SiteIndex);


// VARIABLES
protected:
    // A list of all the site variations the switcher will cycle through. NOTE: Each name must match the name of the sub-level exactly!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Site Variation List")
	TArray<FName> SiteVariations;

private:
	FLatentActionInfo LatentInfo;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	
	int32 CurrentSiteIndex = 0;

};
