#ifndef _RIVE_DATA_CONVERTER_GROUP_HPP_
#define _RIVE_DATA_CONVERTER_GROUP_HPP_
#include "rive/generated/data_bind/converters/data_converter_group_base.hpp"
#include "rive/data_bind/converters/data_converter_group_item.hpp"
#include "rive/data_bind/data_bind.hpp"
#include <stdio.h>
namespace rive
{
class DataConverterGroup : public DataConverterGroupBase
{
public:
    DataValue* convert(DataValue* value, DataBind* dataBind) override;
    DataValue* reverseConvert(DataValue* value, DataBind* dataBind) override;
    void addItem(DataConverterGroupItem* item);
    DataType outputType() override
    {
        if (m_items.size() > 0)
        {
            return m_items.back()->converter()->outputType();
        };
        return Super::outputType();
    }
    const std::vector<DataConverterGroupItem*>& items() { return m_items; }

private:
    std::vector<DataConverterGroupItem*> m_items;
};
} // namespace rive

#endif