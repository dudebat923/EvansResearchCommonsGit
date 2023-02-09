
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PreswerxCamera.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "PreswerxCameraSwitcher.generated.h"

UCLASS()
class PRESWERXSWITCHER_API APreswerxCameraSwitcher : public AActor
{
	GENERATED_BODY()

// FUNCTIONS
public:	
	// Sets default values for this actor's properties
	APreswerxCameraSwitcher();

protected:
	// Code that runs in the editor. In this case, it's used to spawn a PreswerxCamera whenever an element is added to the PreswerxCameras array.
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame (Tick is disabled for this class).
	virtual void Tick(float DeltaTime) override;

protected:
	// Determines if there is already a PreswerxCameraSwitcherActor in the scene. If there is, this one will destroy itself.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void CheckForOtherPreswerxCameraSwitchers();

	// Spawns Preswerx Cameras and adds them to the PreswerxCameras array.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void SpawnPreswerxCameras();
	
	// Switches the view target to the next camera in the PreswerxCameras array.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void GoToNextCamera();

	// Switches the view target to the previous camera in the PreswerxCameras array.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void GoToPreviousCamera();

	// Allows the client to freely fly around the scene.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void GoToFreeCamera();

	// Returns the client to the last PreswerxCamera they were using before they entered Free Camera Mode.
	UFUNCTION(BlueprintCallable, Category = "PreswerxCameraSwitcher")
	void ReturnToFixedCamera();

private:
	// Locks/unlocks the ability to transition between cameras.
	UFUNCTION()
	void SetCanTransition(bool bNewCanTransition);

	// Locks the ability to switch cameras while a transition is in progress. Unlocks it once the transition is complete.
	void LockSwitcherDuringTransition();

	// Sets the character and controller transform equal to the current camera's transform.
	UFUNCTION()
	void MoveControllerToCamera();


// VARIABLES
protected:
	// A customizable list of PreswerxCameras in the scene.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera List")
	TArray<APreswerxCamera*> PreswerxCameras;

	// The time in seconds it will take to transition between cameras. NOTE: The longer the time, the smoother it will look, but you will be unable to switch cameras or enter Free Camera Mode until the transition is complete.
	UPROPERTY(EditAnywhere, Category = "Camera Transition Settings")
	float CameraTransitionTime = 1.0;

	// How the client will transition between cameras. In general, "VTBlend Ease In Out" works the best.
	UPROPERTY(EditAnywhere, Category = "Camera Transition Settings")
	TEnumAsByte<EViewTargetBlendFunction> CameraTransitionType = VTBlend_EaseInOut;

	// Used by certain Camera Transition Types to control the speed of the transition at different points. In general, "1" works the best.
	UPROPERTY(EditAnywhere, Category = "Camera Transition Settings")
	float CameraTransitionExponent = 1.0;

private:
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	ACharacter* PlayerCharacter;
	
	FTimerHandle CameraTransitionTimerHandle;
	FTimerDelegate CameraTransitionTimerDelegate;

	FTimerHandle MoveControllerToCameraTimerHandle;

	int32 CurrentCameraIndex;

	// If false, the client will be unable to switch cameras or enter Free Camera Mode.
	bool bCanTransition = true; 
};
