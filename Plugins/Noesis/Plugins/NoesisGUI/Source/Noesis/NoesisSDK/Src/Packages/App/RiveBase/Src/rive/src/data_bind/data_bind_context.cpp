#include "rive/data_bind_flags.hpp"
#include "rive/data_bind/data_bind_context.hpp"
#include "rive/data_bind/context/context_value_number.hpp"
#include "rive/data_bind/context/context_value_string.hpp"
#include "rive/data_bind/context/context_value_enum.hpp"
#include "rive/data_bind/context/context_value_list.hpp"
#include "rive/data_bind/context/context_value_color.hpp"
#include "rive/data_bind/converters/data_converter_operation_viewmodel.hpp"
#include "rive/data_bind/converters/data_converter_group.hpp"
#include "rive/artboard.hpp"
#include "rive/generated/core_registry.hpp"
#include <iostream>

using namespace rive;

void DataBindContext::decodeSourcePathIds(Span<const uint8_t> value)
{
    BinaryReader reader(value);
    while (!reader.reachedEnd())
    {
        auto val = reader.readVarUintAs<uint32_t>();
        m_SourcePathIdsBuffer.push_back(val);
    }
}

void DataBindContext::copySourcePathIds(const DataBindContextBase& object)
{
    m_SourcePathIdsBuffer = object.as<DataBindContext>()->m_SourcePathIdsBuffer;
}

void DataBindContext::addConverterDependencies(DataConverter* converter,
                                               DataContext* dataContext)
{
    // Look for all converters that target other view model properties and add
    // itself as a dependency of them.
    if (converter != nullptr)
    {
        if (converter->is<DataConverterOperationViewModel>())
        {

            auto value = dataContext->getViewModelProperty(
                converter->as<DataConverterOperationViewModel>()
                    ->sourcePathIds());
            if (value != nullptr)
            {
                value->addDependent(this);
            }
        }
        else if (converter->is<DataConverterGroup>())
        {
            auto items = converter->as<DataConverterGroup>()->items();
            for (auto item : items)
            {
                addConverterDependencies(item->converter(), dataContext);
            }
        }
    }
}

void DataBindContext::bindFromContext(DataContext* dataContext)
{
    if (dataContext != nullptr)
    {
        auto value = dataContext->getViewModelProperty(m_SourcePathIdsBuffer);
        if (value != nullptr)
        {
            value->addDependent(this);
            m_Source = value;
            bind();
        }
        m_dataContext = dataContext;
        if (m_dataConverter != nullptr)
        {
            addConverterDependencies(m_dataConverter, dataContext);
        }
    }
}

void DataBindContext::unbind()
{
    if (m_Source != nullptr)
    {
        m_Source->removeDependent(this);
        m_Source = nullptr;
    }
    DataBind::unbind();
}