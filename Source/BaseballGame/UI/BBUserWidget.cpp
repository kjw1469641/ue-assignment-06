// Copyright Epic Games, Inc. All Rights Reserved.

#include "BBUserWidget.h"

#include "BaseballGame/Player/BBPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

class UTextBlock;

void UBBUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!IsValid(SubmitButton)) return;
	
	SubmitButton->OnClicked.AddUniqueDynamic(this, &UBBUserWidget::OnSubmitButtonClicked);
}

void UBBUserWidget::OnSubmitButtonClicked()
{
	if (!IsValid(InputTextBox)) return;
	
	FString Input = InputTextBox->GetText().ToString();
	
	ABBPlayerController* PlayerController =
	Cast<ABBPlayerController>(GetOwningPlayer());

	if (!IsValid(PlayerController))	return;
	if (Input.IsEmpty()) return;
	PlayerController->SubmitInput(Input);
	
	InputTextBox->SetText(FText::GetEmpty());
	InputTextBox->SetKeyboardFocus();
}

void UBBUserWidget::AddChatMessage(const FString& Message)
{
	if (!IsValid(ChatScrollBox) || !IsValid(WidgetTree))
	{
		return;
	}

	UTextBlock* MessageText =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	if (!IsValid(MessageText))
	{
		return;
	}

	MessageText->SetText(FText::FromString(Message));
	ChatScrollBox->AddChild(MessageText);
	ChatScrollBox->ScrollToEnd();
}

void UBBUserWidget::ShowGameResult(const FString& ResultMessage)
{
	if (!IsValid(ResultOverlay) || !IsValid(ResultTextBlock))
	{
		return;
	}

	ResultTextBlock->SetText(FText::FromString(ResultMessage));
	ResultOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UBBUserWidget::HideGameResult()
{
	if (!IsValid(ResultOverlay))
	{
		return;
	}

	ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
}
