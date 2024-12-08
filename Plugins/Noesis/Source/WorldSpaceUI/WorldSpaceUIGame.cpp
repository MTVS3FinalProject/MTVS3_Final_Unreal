////////////////////////////////////////////////////////////////////////////////////////////////////
// Noesis Engine - http://www.noesisengine.com
// Copyright (c) 2009-2010 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WorldSpaceUIGame.h"
#include "Modules/ModuleManager.h"

#include "PercentageConverter.h"

class WorldSpaceUIModule: public FDefaultGameModuleImpl
{
    virtual void StartupModule() override
    {
        Noesis::RegisterComponent<WorldSpaceUI::PercentageConverter>();
    }

    virtual void ShutdownModule() override
    {
        Noesis::UnregisterComponent<WorldSpaceUI::PercentageConverter>();
    }
};

IMPLEMENT_MODULE(WorldSpaceUIModule, WorldSpaceUIModule);