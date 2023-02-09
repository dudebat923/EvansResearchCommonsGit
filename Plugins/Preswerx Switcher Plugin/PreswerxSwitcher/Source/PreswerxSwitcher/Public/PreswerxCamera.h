
#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/Actor.h"
#include "PreswerxCamera.generated.h"

UCLASS()
class PRESWERXSWITCHER_API APreswerxCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	APreswerxCamera();

	void SetCameraName(FString NewName, int32 Number);

	FString GetCameraName();

private:
	FString CameraName = "Camera_01";
};
