// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"

class UBBUserWidget;

UCLASS()
class BASEBALLGAME_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SubmitInput(const FString& Input);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCReceiveMessage(const FString& Message);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameResult(const FString& ResultMessage);

	UFUNCTION(Client, Reliable)
	void ClientRPCHideGameResult();
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BaseballGame")
	TSubclassOf<UBBUserWidget> UserWidgetClass;
	
	UPROPERTY()
	UBBUserWidget* UserWidgetInstance = nullptr;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCSubmitInput(const FString& Input);
};
