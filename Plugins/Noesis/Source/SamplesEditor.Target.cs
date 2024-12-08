////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

using UnrealBuildTool;
using System.Collections.Generic;

public class SamplesEditorTarget : TargetRules
{
    public SamplesEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        ExtraModuleNames.Add("DataBindingModule");
        ExtraModuleNames.Add("InventoryModule");
        ExtraModuleNames.Add("LocalizeModule");
        ExtraModuleNames.Add("LoginModule");
        ExtraModuleNames.Add("UserControlModule");
        ExtraModuleNames.Add("WorldSpaceUIModule");
        ExtraModuleNames.Add("SamplesModule");

        DefaultBuildSettings = BuildSettingsVersion.Latest;
        WindowsPlatform.bStrictConformanceMode = true;
        CppStandard = CppStandardVersion.Default;

#if UE_5_1_OR_LATER
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif

#if UE_5_3_OR_LATER
        bLegacyParentIncludePaths = false;
#endif

#if UE_5_4_OR_LATER
        bValidateFormatStrings = true;
#endif
    }
}
