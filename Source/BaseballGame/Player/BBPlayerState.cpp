// Fill out your copyright notice in the Description page of Project Settings.


#include "BBPlayerState.h"

#include "Net/UnrealNetwork.h"

void ABBPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBPlayerState, CurrentAttempts);
	DOREPLIFETIME(ABBPlayerState, MaxAttempts);
}

int32 ABBPlayerState::GetCurrentAttempts() const
{
	return CurrentAttempts;
}

int32 ABBPlayerState::GetMaxAttempts() const
{
	return MaxAttempts;
}

void ABBPlayerState::IncreaseAttempts()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentAttempts = FMath::Min(CurrentAttempts + 1, MaxAttempts);
}

void ABBPlayerState::ResetAttempts()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentAttempts = 0;
}

bool ABBPlayerState::HasUsedAllAttempts() const
{
    return CurrentAttempts >= MaxAttempts;
}
