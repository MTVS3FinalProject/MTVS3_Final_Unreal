////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LocalizeGame.h"
#include "LevelToColorConverter.h"

DEFINE_LOG_CATEGORY(LogNoesisLocalize)

class LocalizeModule : public FDefaultGameModuleImpl
{
    virtual void StartupModule() override
    {
        Noesis::RegisterComponent<Localize::LevelToColorConverter>();
    }

    virtual void ShutdownModule() override
    {
        Noesis::UnregisterComponent<Localize::LevelToColorConverter>();
    }
};

IMPLEMENT_MODULE(LocalizeModule, LocalizeModule);
