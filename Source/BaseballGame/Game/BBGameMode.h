// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BBGameMode.generated.h"

class ABBPlayerController;
UCLASS()
class BASEBALLGAME_API ABBGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void HandlePlayerInput(ABBPlayerController* PlayerController, const FString& Input);
	
	UFUNCTION()
	bool IsValidInput(const FString& Input) const;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	void ShowGameResultForAll(const FString& ResultMessage);
	void HideGameResultForAll();
protected:
	FString Answer;
	
	UFUNCTION()
	void GenerateRandomNumbers();
	
	UFUNCTION()
	FString CheckAnswer(const FString& Input) const;
	
	UFUNCTION()
	void CheckGameResult(ABBPlayerController* PlayerController, const FString& Result);
	
	UFUNCTION()
	bool AreAllPlayersOutOfAttempts() const;
	
	void BroadcastMessage(const FString& Message);
	
	bool GameInProgress = true;
	
	FTimerHandle ResetTimerHandle;
	
	void EndGame(const FString& ResultMessage);
	
	void ResetGame();
	
	int32 NextPlayerNumber = 1;
};

