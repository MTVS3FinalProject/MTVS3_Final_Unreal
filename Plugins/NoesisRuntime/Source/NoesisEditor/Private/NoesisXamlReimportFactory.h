////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// UnrealEd includes
#include "EditorReimportHandler.h"

// NoesisEditor includes
#include "NoesisXamlFactory.h"

// Generated header include
#include "NoesisXamlReimportFactory.generated.h"

UCLASS()
class NOESISEDITOR_API UNoesisXamlReimportFactory : public UNoesisXamlFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	// FReimportHandler interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	// End of FReimportHandler interface
};
