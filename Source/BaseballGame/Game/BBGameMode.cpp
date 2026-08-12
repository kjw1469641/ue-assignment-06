// Fill out your copyright notice in the Description page of Project Settings.


#include "BBGameMode.h"

#include "BaseballGame/Player/BBPlayerController.h"
#include "BaseballGame/Player/BBPlayerState.h"
#include "TimerManager.h"
#include "GameFramework/PlayerState.h"

void ABBGameMode::BeginPlay()
{
	Super::BeginPlay();
	GenerateRandomNumbers();
}

void ABBGameMode::HandlePlayerInput(ABBPlayerController* PlayerController, const FString& Input)
{
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	if (GameInProgress == false)
	{
		PlayerController->ClientRPCReceiveMessage(TEXT("게임 재시작을 기다려주세요.")); 
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[GameMode Received] Input: %s"), *Input);
	
	ABBPlayerState* PlayerState = PlayerController->GetPlayerState<ABBPlayerState>();
	if (!IsValid(PlayerState)) return;
	
	if (PlayerState->HasUsedAllAttempts())
	{
		PlayerController->ClientRPCReceiveMessage(TEXT("기회를 모두 사용했습니다."));
		return;
	}
	
	if (!IsValidInput(Input))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Invalid Input] 다시 입력하세요: %s"), *Input);
		PlayerController->ClientRPCReceiveMessage(TEXT("다시 입력하세요"));
		return;
	}
	
	PlayerState->IncreaseAttempts();
	
	UE_LOG(LogTemp, Log, TEXT("[Valid Input] Input: %s"), *Input);
	
	const FString Result = CheckAnswer(Input);
	
	UE_LOG(LogTemp, Log, TEXT("[Judgement] Input: %s | Result: %s"), *Input, *Result);
	
	const FString ToClient = FString::Printf(TEXT("%s : %s [%d / %d]"), *Input, *Result, PlayerState->GetCurrentAttempts(), PlayerState->GetMaxAttempts());
	
	BroadcastMessage(ToClient);
	CheckGameResult(PlayerController, Result);
}

bool ABBGameMode::IsValidInput(const FString& Input) const
{
	if (Input.Len() != 3)
	{
		return false;
	}

	TSet<TCHAR> UsedDigits;

	for (const TCHAR Digit : Input)
	{
		if (Digit < TEXT('1') || Digit > TEXT('9'))
		{
			return false;
		}

		if (UsedDigits.Contains(Digit))
		{
			return false;
		}

		UsedDigits.Add(Digit);
	}

	return true;
}

void ABBGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!IsValid(NewPlayer))
	{
		return;
	}

	APlayerState* JoinedPlayerState = NewPlayer->PlayerState;

	if (!IsValid(JoinedPlayerState))
	{
		return;
	}

	const FString PlayerName =
		FString::Printf(TEXT("Player %d"), NextPlayerNumber);

	JoinedPlayerState->SetPlayerName(PlayerName);
	++NextPlayerNumber;
}

void ABBGameMode::ShowGameResultForAll(const FString& ResultMessage)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	 Iterator; ++Iterator)
	{
		APlayerController* RPCPlayerController = Iterator->Get();
		
		ABBPlayerController* BaseballController =
	   Cast<ABBPlayerController>(RPCPlayerController);
		
		if (!IsValid(BaseballController))
		{
			continue;
		}

		BaseballController->ClientRPCShowGameResult(ResultMessage);
	}
}

void ABBGameMode::HideGameResultForAll()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	 Iterator; ++Iterator)
	{
		APlayerController* RPCPlayerController = Iterator->Get();
		
		ABBPlayerController* BaseballController =
	   Cast<ABBPlayerController>(RPCPlayerController);
		
		if (!IsValid(BaseballController))
		{
			continue;
		}

		BaseballController->ClientRPCHideGameResult();
	}
}

void ABBGameMode::GenerateRandomNumbers()
{
	TArray<int32> Numbers;
	for (int32 Number = 1; Number <= 9; ++Number)
	{
		Numbers.Add(Number);
	}
	
	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		const int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	
	Answer = Result;
	UE_LOG(LogTemp, Log, TEXT("[Secret Answer] %s"), *Result);
}

FString ABBGameMode::CheckAnswer(const FString& Input) const
{
	int32 Strike = 0;
	int32 Ball = 0;
	
	for (int32 i = 0; i < 3; i++)
	{
		if (Input[i] == Answer[i])
			++Strike;
		else if (Answer.Contains(FString::Chr(Input[i])))
			++Ball;
	}
	
	if (Strike == 0 && Ball == 0)
		return TEXT("OUT");
	else if (Strike > 0 && Ball == 0)
		return FString::Printf(TEXT("%dS"), Strike);
	else if (Strike == 0 && Ball > 0)
		return FString::Printf(TEXT("%dB"), Ball);
	else
		return FString::Printf(TEXT("%dS%dB"), Strike, Ball);
}

void ABBGameMode::CheckGameResult(ABBPlayerController* PlayerController, const FString& Result)
{
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	if (Result == TEXT("3S"))
	{
		ABBPlayerState* WinnerPlayerState = PlayerController->GetPlayerState<ABBPlayerState>();
		if (!IsValid(WinnerPlayerState))
		{
			return;
		}

		const FString WinnerName = WinnerPlayerState->GetPlayerName();
		const FString WinnerMessage = FString::Printf(TEXT("%s 승리!"), *WinnerName);
		EndGame(WinnerMessage);
		return;
	}

	if (AreAllPlayersOutOfAttempts())
	{
		EndGame(TEXT("무승부!"));
	}
}

bool ABBGameMode::AreAllPlayersOutOfAttempts() const
{
	bool bFoundPlayer = false;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	 Iterator; ++Iterator)
	{
		ABBPlayerController* BaseballController =
			Cast<ABBPlayerController>(Iterator->Get());

		if (!IsValid(BaseballController))
		{
			continue;
		}

		ABBPlayerState* PlayerState = BaseballController->GetPlayerState<ABBPlayerState>();
		if (!IsValid(PlayerState))
		{
			continue;
		}

		bFoundPlayer = true;
		
		if (!PlayerState->HasUsedAllAttempts())
		{
			return false;
		}
	}

	return bFoundPlayer;
}

void ABBGameMode::BroadcastMessage(const FString& Message)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
	 Iterator; ++Iterator)
	{
		APlayerController* RPCPlayerController = Iterator->Get();
		
		ABBPlayerController* BaseballController =
	   Cast<ABBPlayerController>(RPCPlayerController);
		
		if (!IsValid(BaseballController))
		{
			continue;
		}

		BaseballController->ClientRPCReceiveMessage(Message);
	}
}



void ABBGameMode::EndGame(const FString& ResultMessage)
{
	if (GameInProgress == false) return;
	
	GameInProgress = false;
	
	BroadcastMessage(ResultMessage);
	ShowGameResultForAll(ResultMessage);
	
	GetWorldTimerManager().SetTimer(ResetTimerHandle,
		this,
		&ABBGameMode::ResetGame,
		3.0f,
		false);
}

void ABBGameMode::ResetGame()
{
	for (FConstPlayerControllerIterator Iterator =
			 GetWorld()->GetPlayerControllerIterator();
		 Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (!IsValid(PlayerController))
		{
			continue;
		}

		ABBPlayerState* PlayerState =
			PlayerController->GetPlayerState<ABBPlayerState>();

		if (!IsValid(PlayerState))
		{
			continue;
		}

		PlayerState->ResetAttempts();
	}

	GenerateRandomNumbers();
	GameInProgress = true;

	HideGameResultForAll();
	BroadcastMessage(TEXT("새 게임을 시작합니다!"));
}
