#pragma once

UENUM(BlueprintType)
enum class ERPGLocomotionDirection : uint8
{
	None,
	Forward,
	Backward,
	Left,
	Right,
	Forward_Left,
	Forward_Right,
	Backward_Left,
	Backward_Right
};