
#include "PreswerxCamera.h"

APreswerxCamera::APreswerxCamera()
{

}

void APreswerxCamera::SetCameraName(FString NewName, int32 Number)
{
	CameraName = NewName;

	CameraName.AppendInt(Number);
}

FString APreswerxCamera::GetCameraName()
{
	return CameraName;
}
