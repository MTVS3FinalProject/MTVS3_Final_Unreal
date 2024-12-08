////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NoesisRuntime.h"

namespace Localize
{

    class LevelToColorConverter : public Noesis::BaseValueConverter
    {
    public:
        bool TryConvert(Noesis::BaseComponent* value, const Noesis::Type* targetType, Noesis::BaseComponent*,
            Noesis::Ptr<Noesis::BaseComponent>& result)
        {
            if (targetType == Noesis::TypeOf<Noesis::Color>())
            {
                float level;
                if (Noesis::Boxing::CanUnbox<float>(value)) level = Noesis::Boxing::Unbox<float>(value);
                else if (Noesis::Boxing::CanUnbox<double>(value)) level = (float)Noesis::Boxing::Unbox<double>(value);
                else return false;

                float perc = level / 100.0f;
                int red = (int)(255 * Noesis::Min(1.0f, 0.4f + 1.0f - perc));
                int green = (int)(255 * Noesis::Min(1.0f, 0.4f + perc));

                result = Noesis::Boxing::Box(Noesis::Color(red, green, 0));
                return true;
            }

            return false;
        }

    private:
        NS_IMPLEMENT_INLINE_REFLECTION_(LevelToColorConverter, Noesis::BaseValueConverter, "Localize.LevelToColorConverter")
    };

}
