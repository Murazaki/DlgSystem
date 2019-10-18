// Copyright 2017-2018 Csaba Molnar, Daniel Butum
#include "DlgSystemSettings.h"

#include "GameFramework/Character.h"

#include "DlgManager.h"
#include "Logging/DlgLogger.h"

#define LOCTEXT_NAMESPACE "DlgSystemSettings"

//////////////////////////////////////////////////////////////////////////
// UDlgSystemSettings
const FText UDlgSystemSettings::EdgeTextFinish = LOCTEXT("edge_finish", "Finish");
const FText UDlgSystemSettings::EdgeTextNext = LOCTEXT("edge_next", "Next");

UDlgSystemSettings::UDlgSystemSettings()
{
	BlacklistedReflectionClasses = {AActor::StaticClass(), APawn::StaticClass(),  ACharacter::StaticClass()};
	// AdditionalTextFormatFileExtensionsToLookFor = {""};
}

#if WITH_EDITOR
FText UDlgSystemSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "Dialogue");
}

FText UDlgSystemSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Configure how the Dialogue Editor behaves + Runtime behaviour");
}

bool UDlgSystemSettings::CanEditChange(const UProperty* InProperty) const
{
	const bool bIsEditable = Super::CanEditChange(InProperty);
	if (bIsEditable && InProperty)
	{
		const FName PropertyName = InProperty->GetFName();

		// Do now allow to change the bDrawPrimaryEdges, bDrawSecondaryEdges if we aren't even showing them
		if (!bShowPrimarySecondaryEdges &&
			(PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, bDrawPrimaryEdges) ||
			 PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, bDrawSecondaryEdges)))
		{
			return false;
		}

		// Only useful for GlobalNamespace
		if (DialogueTextNamespaceLocalization != EDlgTextNamespaceLocalization::Global &&
			PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, DialogueTextGlobalNamespaceName))
		{
			return false;
		}
	}

	return bIsEditable;
}

void UDlgSystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, bEnableMessageLog))
	{
		// Prevent no logging at all
		bEnableOutputLog = !bEnableMessageLog;
	}

	// Check category
	if (PropertyChangedEvent.Property != nullptr && PropertyChangedEvent.Property->HasMetaData(TEXT("Category")))
	{
		const FString& Category = PropertyChangedEvent.Property->GetMetaData(TEXT("Category"));

		// Sync logger settings
		if (Category.Equals(TEXT("Logger"), ESearchCase::IgnoreCase))
		{
			FDlgLogger::Get().SyncWithSettings();
		}
	}
}
#endif // WITH_EDITOR

FString UDlgSystemSettings::GetTextFileExtension(EDlgDialogueTextFormat TextFormat)
{
	switch (TextFormat)
	{
		// JSON has the .json added at the end
		case EDlgDialogueTextFormat::JSON:
			return TEXT(".dlg.json");

		case EDlgDialogueTextFormat::DialogueDEPRECATED:
			return TEXT(".dlg");

		// Empty
		case EDlgDialogueTextFormat::None:
		default:
			return FString();
	}
}
 
const TSet<FString>& UDlgSystemSettings::GetAllCurrentTextFileExtensions()
{
	static TSet<FString> Extensions;
	if (Extensions.Num() == 0)
	{
		// Iterate over all possible text formats
		const int32 TextFormatsNum = static_cast<int32>(EDlgDialogueTextFormat::NumTextFormats);
		for (int32 TextFormatIndex = static_cast<int32>(EDlgDialogueTextFormat::StartTextFormats);
				   TextFormatIndex < TextFormatsNum; TextFormatIndex++)
		{
			const EDlgDialogueTextFormat CurrentTextFormat = static_cast<EDlgDialogueTextFormat>(TextFormatIndex);
			Extensions.Add(GetTextFileExtension(CurrentTextFormat));
		}
	}

	return Extensions;
}

TSet<FString> UDlgSystemSettings::GetAllTextFileExtensions() const
{
	TSet<FString> CurrentFileExtensions = GetAllCurrentTextFileExtensions();

	// Look for additional file extensions
	for (const FString& Ext : AdditionalTextFormatFileExtensionsToLookFor)
	{
		// Only allow file extension that start with dot, also ignore uasset
		if (Ext.StartsWith(".") && Ext != ".uasset")
		{
			CurrentFileExtensions.Add(Ext);
		}
	}

	return CurrentFileExtensions;
}



#undef LOCTEXT_NAMESPACE
