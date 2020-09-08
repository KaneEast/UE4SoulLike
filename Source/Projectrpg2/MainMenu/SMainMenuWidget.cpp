
// Fill out your copyright notice in the Description page of Project Settings.


#include "SMainMenuWidget.h"
#include "MenuHUD.h"
#include <Kismet/GameplayStatics.h>
#include "RPGMainMenuPlayerController.h"
#include "../ToolsClass/RPGTollsClass.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	OwningHUD = InArgs._OwningHUD;

	const FMargin ContentPadding = FMargin(500.f, 300.f);
	const FMargin ButtonPadding = FMargin(10.f);

	const FText TitleText = LOCTEXT("GameTitle", "My Action RPG Game");
	const FText PlayText = LOCTEXT("PlayGame", "Play");
	const FText SettingsText = LOCTEXT("Settings", "Settings");
	const FText QuitText = LOCTEXT("QuitGame", "Quit Game");

	//FSlateFontInfo FontInfo_Button = FCoreStyle::Get().GetFontStyle("Go_2_Old_Western_Font", "Assets/Font");// EmbossedText
	FSlateFontInfo FontInfo_Button = FCoreStyle::Get().GetFontStyle("EmbossedText");// EmbossedText

	FontInfo_Button.Size = 40.f;


	FSlateFontInfo FontInfo_Title = FontInfo_Button;
	FontInfo_Title.Size = 60.f;


	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FColor::Black)
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(ContentPadding)
		[
			SNew(SVerticalBox)

			// Title text
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Font(FontInfo_Title)
				.Text(TitleText)
				.Justification(ETextJustify::Center)
				
			]

			// Play button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnPlayClicked)
				[
					SNew(STextBlock)
					.Font(FontInfo_Button)
					.Text(PlayText)
					.Justification(ETextJustify::Center)
				]
			]

			// Settings button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				[
					SNew(STextBlock)
					.Font(FontInfo_Button)
					.Text(SettingsText)
					.Justification(ETextJustify::Center)
				]
			]

			// Quit button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnQuitCliked)
				[
					SNew(STextBlock)
					.Font(FontInfo_Button)
					.Text(QuitText)
					.Justification(ETextJustify::Center)
				]
			]
		]
	];
}

FReply SMainMenuWidget::OnPlayClicked() const
{
	if (OwningHUD.IsValid())
	{
		//RPGTollsClass::OpenLevel(FName("SunTemple"));
		if (ARPGMainMenuPlayerController* PC = Cast<ARPGMainMenuPlayerController>(OwningHUD->PlayerOwner))
		{
			PC->OpenLevel(FName("SunTemple"));
		}
	}

	return FReply::Handled();
}

FReply SMainMenuWidget::OnQuitCliked() const
{
	if (OwningHUD.IsValid())
	{
		if (APlayerController* PC = OwningHUD->PlayerOwner)
		{
			PC->ConsoleCommand("quit");
		}
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE