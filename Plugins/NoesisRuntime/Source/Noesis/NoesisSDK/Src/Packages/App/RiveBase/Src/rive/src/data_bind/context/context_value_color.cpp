#include "rive/data_bind/context/context_value_color.hpp"
#include "rive/data_bind/data_values/data_value_color.hpp"
#include "rive/generated/core_registry.hpp"

using namespace rive;

DataBindContextValueColor::DataBindContextValueColor(DataBind* dataBind) :
    DataBindContextValue(dataBind)
{}

void DataBindContextValueColor::apply(Core* target,
                                      uint32_t propertyKey,
                                      bool isMainDirection)
{
    updateSourceValue();
    auto value = calculateValue<DataValueColor, int>(m_dataValue,
                                                     isMainDirection,
                                                     m_dataBind);
    CoreRegistry::setColor(target, propertyKey, value);
}

DataValue* DataBindContextValueColor::getTargetValue(Core* target,
                                                     uint32_t propertyKey)
{
    auto value = CoreRegistry::getColor(target, propertyKey);
    return new DataValueColor(value);
}