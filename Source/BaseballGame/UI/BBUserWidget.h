// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBUserWidget.generated.h"

class UEditableTextBox;
class UButton;
class UScrollBox;
class UOverlay;
class UTextBlock;

UCLASS()
class BASEBALLGAME_API UBBUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void AddChatMessage(const FString& Message);
	
	void ShowGameResult(const FString& ResultMessage);
	void HideGameResult();
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> InputTextBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SubmitButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;
	
	UFUNCTION()
	void OnSubmitButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ResultOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultTextBlock;
};

