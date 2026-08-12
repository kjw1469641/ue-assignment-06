// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BBPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABBPlayerState : public APlayerState
{
	GENERATED_BODY()
		
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	int32 GetCurrentAttempts() const;
	
	int32 GetMaxAttempts() const;
	
	void IncreaseAttempts();
	
	void ResetAttempts();
	
	bool HasUsedAllAttempts() const;
	
protected:
	UPROPERTY(Replicated)
	int32 CurrentAttempts = 0;
	UPROPERTY(Replicated)
	int32 MaxAttempts = 3;
};
