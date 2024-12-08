////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "OrbitConverter.h"
#include "NoesisSDK.h"

using namespace DataBinding;
using namespace Noesis;


////////////////////////////////////////////////////////////////////////////////////////////////////
bool OrbitConverter::TryConvert(BaseComponent* value, const Type*,
    BaseComponent* parameter, Noesis::Ptr<BaseComponent>& result)
{
    float orbit;
    if (Boxing::CanUnbox<float>(value)) orbit = Boxing::Unbox<float>(value);
    else if (Boxing::CanUnbox<double>(value)) orbit = (float)Boxing::Unbox<double>(value);
    else return false;

    if (!Boxing::CanUnbox<String>(parameter)) return false;
    float factor = (float)atof(Boxing::Unbox<String>(parameter).Str());

    result = Boxing::Box(powf(orbit / 40, 0.4f) * 770.0f * factor);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(OrbitConverter, "DataBinding.OrbitConverter")

NS_END_COLD_REGION