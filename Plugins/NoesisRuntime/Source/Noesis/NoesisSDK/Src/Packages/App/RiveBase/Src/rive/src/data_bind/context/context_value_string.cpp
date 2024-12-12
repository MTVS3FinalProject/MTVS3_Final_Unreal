#include "rive/data_bind/context/context_value_string.hpp"
#include "rive/data_bind/data_values/data_value_string.hpp"
#include "rive/generated/core_registry.hpp"

using namespace rive;

DataBindContextValueString::DataBindContextValueString(DataBind* dataBind) :
    DataBindContextValue(dataBind)
{}

void DataBindContextValueString::apply(Core* target,
                                       uint32_t propertyKey,
                                       bool isMainDirection)
{
    updateSourceValue();
    auto value = calculateValue<DataValueString, std::string>(m_dataValue,
                                                              isMainDirection,
                                                              m_dataBind);
    CoreRegistry::setString(target, propertyKey, value);
}

DataValue* DataBindContextValueString::getTargetValue(Core* target,
                                                      uint32_t propertyKey)
{
    auto value = CoreRegistry::getString(target, propertyKey);
    return new DataValueString(value);
}