////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoginFunctionLibrary.h"

// Noesis includes
#include "NoesisRuntime.h"

ULoginFunctionLibrary::ULoginFunctionLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool ULoginFunctionLibrary::IsPasswordValid(UObject* InPasswordBox, FString AccountName)
{
    Noesis::Ptr<Noesis::PasswordBox> PasswordBox = Noesis::DynamicPtrCast<Noesis::PasswordBox>(NoesisCreateComponentForUObject(InPasswordBox));
    if (PasswordBox != nullptr)
    {
        return (FCStringAnsi::Strcmp("12345", PasswordBox->GetPassword()) == 0) && (AccountName == TEXT("noesis"));
    }

    return false;
}
