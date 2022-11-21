// Copyright (C) Developed by Pask, Published by Dark Tower Interactive SRL 2021. All Rights Reserved. 


#include "CASMaster.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCASMaster::UCASMaster()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCASMaster::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCASMaster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCASMaster::MulticastPlayAnimMontage_Implementation(class UAnimMontage* AnimMontage, class ACharacter* character)
{
	if (!character) {
		return;
	}
	UAnimInstance* animinst = character->GetMesh()->GetAnimInstance();
	if (animinst) {
		animinst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
		character->PlayAnimMontage(AnimMontage);
	}
}

bool UCASMaster::MulticastPlayAnimMontage_Validate(class UAnimMontage* AnimMontage, class ACharacter* character)
{
	return true;
}
