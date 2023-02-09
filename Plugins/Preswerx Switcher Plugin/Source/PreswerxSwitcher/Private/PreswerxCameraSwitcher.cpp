
#include "PreswerxCameraSwitcher.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APreswerxCameraSwitcher::APreswerxCameraSwitcher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CheckForOtherPreswerxCameraSwitchers();
}

// Code that runs in the editor. In this case, it's used to spawn a PreswerxCamera whenever an element is added to the PreswerxCameras array.
void APreswerxCameraSwitcher::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CheckForOtherPreswerxCameraSwitchers();
	
	SpawnPreswerxCameras();
}

// Called when the game starts or when spawned
void APreswerxCameraSwitcher::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	CameraTransitionTimerDelegate.BindUObject(this, &APreswerxCameraSwitcher::SetCanTransition, true);

	// Sets the first camera in the PreswerxCameras array as the view target.
	if (PreswerxCameras.Num()>0 && PreswerxCameras[0] && PlayerController)
	{
		CurrentCameraIndex = 0;
		MoveControllerToCamera();
	}
}

// Called every frame (Tick is disabled for this class).
void APreswerxCameraSwitcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Determines if there is already a PreswerxCameraSwitcherActor in the scene. If there is, this one will destroy itself.
void APreswerxCameraSwitcher::CheckForOtherPreswerxCameraSwitchers()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, APreswerxCameraSwitcher::StaticClass(), FoundActors);
	if (FoundActors.Num() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn. There is already a Preswerx Camera Switcher Actor in the scene."))
		Destroy();
	}
}
// Spawns Preswerx Cameras and adds them to the PreswerxCameras array.
void APreswerxCameraSwitcher::SpawnPreswerxCameras()
{
	for (int32 i = 0; i < PreswerxCameras.Num(); i++)
	{
		const int32 CameraNumber = i + 1;

		if (!PreswerxCameras[i])
		{
			PreswerxCameras[i] = GetWorld()->SpawnActor<APreswerxCamera>(GetActorLocation(), FRotator(0, 0, 0), FActorSpawnParameters());

			PreswerxCameras[i]->SetCameraName(TEXT("PreswerxCamera_"), CameraNumber);
			#if WITH_EDITOR
				PreswerxCameras[i]->SetActorLabel(PreswerxCameras[i]->GetCameraName());
			#endif
		}
		else
		{
			PreswerxCameras[i]->SetCameraName(TEXT("PreswerxCamera_"), CameraNumber);
			#if WITH_EDITOR
				PreswerxCameras[i]->SetActorLabel(PreswerxCameras[i]->GetCameraName());
			#endif
		}
	}
}

// Switches the view target to the next camera in the PreswerxCameras array.
void APreswerxCameraSwitcher::GoToNextCamera()
{
	if (bCanTransition)
	{
		const int32 NextIndex = CurrentCameraIndex + 1;

		if (NextIndex + 1 <= PreswerxCameras.Num() && PreswerxCameras[NextIndex])
		{
			PlayerController->SetViewTargetWithBlend(PreswerxCameras[NextIndex], CameraTransitionTime, CameraTransitionType, CameraTransitionExponent, false);
			CurrentCameraIndex++;
		}
		else if (PreswerxCameras[0])
		{
			PlayerController->SetViewTargetWithBlend(PreswerxCameras[0], CameraTransitionTime, CameraTransitionType, CameraTransitionExponent, false);
			CurrentCameraIndex = 0;
		}
		LockSwitcherDuringTransition();
		GetWorldTimerManager().SetTimer(MoveControllerToCameraTimerHandle, this, &APreswerxCameraSwitcher::MoveControllerToCamera, 1, false, CameraTransitionTime);
	}
}

// Switches the view target to the previous camera in the PreswerxCameras array.
void APreswerxCameraSwitcher::GoToPreviousCamera()
{
	if (bCanTransition)
	{
		const int32 PreviousIndex = CurrentCameraIndex - 1;
		
		if (PreviousIndex >= 0 && PreswerxCameras[PreviousIndex])
		{
			PlayerController->SetViewTargetWithBlend(PreswerxCameras[PreviousIndex], CameraTransitionTime, CameraTransitionType, CameraTransitionExponent, false);
			CurrentCameraIndex--;
		}
		else if (PreswerxCameras[PreswerxCameras.Num() - 1])
		{
			PlayerController->SetViewTargetWithBlend(PreswerxCameras[PreswerxCameras.Num() - 1], CameraTransitionTime, CameraTransitionType, CameraTransitionExponent, false);
			CurrentCameraIndex = PreswerxCameras.Num() - 1;
		}
		LockSwitcherDuringTransition();
		GetWorldTimerManager().SetTimer(MoveControllerToCameraTimerHandle, this, &APreswerxCameraSwitcher::MoveControllerToCamera, 1, false, CameraTransitionTime);
	}
}

// Allows the client to freely fly around the scene.
void APreswerxCameraSwitcher::GoToFreeCamera()
{
	if (bCanTransition)
	{
		const FVector MoveDirection = PreswerxCameras[CurrentCameraIndex]->GetActorForwardVector().GetSafeNormal() * 250.0;
		FVector FreeCameraLoctation = PreswerxCameras[CurrentCameraIndex]->GetActorLocation() + MoveDirection;
		
		PlayerCharacter->SetActorLocationAndRotation(FreeCameraLoctation, PreswerxCameras[CurrentCameraIndex]->GetActorRotation());
		PlayerController->SetControlRotation(PlayerCharacter->GetActorRotation());
		PlayerController->SetViewTargetWithBlend(PlayerController, 0, CameraTransitionType, CameraTransitionExponent, false);
	}
	LockSwitcherDuringTransition();
}

// Returns the client to the last PreswerxCamera they were using before they entered Free Camera Mode.
void APreswerxCameraSwitcher::ReturnToFixedCamera()
{
	if (PreswerxCameras[CurrentCameraIndex])
	{
		PlayerController->SetViewTargetWithBlend(PreswerxCameras[CurrentCameraIndex], CameraTransitionTime, CameraTransitionType, CameraTransitionExponent, false);
	}
	LockSwitcherDuringTransition();
	GetWorldTimerManager().SetTimer(MoveControllerToCameraTimerHandle, this, &APreswerxCameraSwitcher::MoveControllerToCamera, 1, false, CameraTransitionTime);
}

// Locks/unlocks the ability to transition between cameras.
void APreswerxCameraSwitcher::SetCanTransition(bool bNewCanTransition)
{
	bCanTransition = bNewCanTransition;
}

// Locks the ability to switch cameras while a transition is in progress. Unlocks it once the transition is complete.
void APreswerxCameraSwitcher::LockSwitcherDuringTransition()
{
	SetCanTransition(false);
	GetWorldTimerManager().SetTimer(CameraTransitionTimerHandle, CameraTransitionTimerDelegate, 1, false, CameraTransitionTime);
}

// Sets the character and controller transform equal to the current camera's transform.
void APreswerxCameraSwitcher::MoveControllerToCamera()
{
	PlayerCharacter->SetActorLocationAndRotation(PreswerxCameras[CurrentCameraIndex]->GetActorLocation(), PreswerxCameras[CurrentCameraIndex]->GetActorRotation());
	PlayerController->SetControlRotation(PlayerCharacter->GetActorRotation());
	PlayerController->SetViewTargetWithBlend(PlayerController, 0, CameraTransitionType, CameraTransitionExponent, false);
}
