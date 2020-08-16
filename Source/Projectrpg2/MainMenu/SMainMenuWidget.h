// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "CoreMinimal.h"


/**
 * 
 */
class PROJECTRPG2_API SMainMenuWidget :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMainMenuWidget) {}

	SLATE_ARGUMENT(TWeakObjectPtr<class AMenuHUD>, OwningHUD)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply OnPlayClicked() const;
	FReply OnQuitCliked() const;

	TWeakObjectPtr<class AMenuHUD> OwningHUD;

	virtual bool SupportsKeyboardFocus() const override { return true; };

	/*UPROPERTY(EditAnywhere, Category = "RPGFontInfo")
	FSlateFontInfo FontInfo_Title;

	UPROPERTY(EditAnywhere, Category = "RPGFontInfo")
	FSlateFontInfo FontInfo_Button;*/
};
