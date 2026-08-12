// Fill out your copyright notice in the Description page of Project Settings.


#include "BBPlayerController.h"

#include "BaseballGame/Game/BBGameMode.h"
#include "BaseballGame/UI/BBUserWidget.h"


void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController() || !IsValid(UserWidgetClass)) return;
	
	UserWidgetInstance = CreateWidget<UBBUserWidget>(this, UserWidgetClass);
	
	if (!IsValid(UserWidgetInstance)) return;
	
	UserWidgetInstance->AddToViewport();
	
	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);
	
	bShowMouseCursor = true;
}

void ABBPlayerController::SubmitInput(const FString& Input)
{
	UE_LOG(LogTemp, Log, TEXT("[Local Submit] Input: %s"), *Input);
	ServerRPCSubmitInput(Input);
}

void ABBPlayerController::ClientRPCShowGameResult_Implementation(const FString& ResultMessage)
{
	if (!IsValid(UserWidgetInstance))
	{
		return;
	}

	UserWidgetInstance->ShowGameResult(ResultMessage);
}

void ABBPlayerController::ClientRPCHideGameResult_Implementation()
{
	if (!IsValid(UserWidgetInstance))
	{
		return;
	}

	UserWidgetInstance->HideGameResult();
}

void ABBPlayerController::ServerRPCSubmitInput_Implementation(const FString& Input)
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[Server Received] Input: %s | HasAuthority: %s"),
		*Input,
		HasAuthority() ? TEXT("true") : TEXT("false")
	);
	
	ABBGameMode* GameMode = GetWorld()->GetAuthGameMode<ABBGameMode>();
	if (!IsValid(GameMode)) return;
	
	GameMode->HandlePlayerInput(this, Input);
	
}

void ABBPlayerController::ClientRPCReceiveMessage_Implementation(const FString& Message)
{
	if (!IsValid(UserWidgetInstance)) return;
	
	UserWidgetInstance->AddChatMessage(Message);
}
