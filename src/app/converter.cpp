#include "converter.h"

#include <stdexcept>

Pipeline CmdLineArgs2PipelineConverter::createPipeline(
    const std::vector<FilterDescriptor>& descriptors) const
{
    Pipeline pipeline;

    for(const FilterDescriptor& descriptor: descriptors)
    {
        FilterProducer producer = getFilterProducer(descriptor.name);
        if(!producer)
            throw std::invalid_argument("Неизвестный фильтр: " +
                                        descriptor.name);

        IFilter* filter = producer(descriptor);
        if(!filter)
            throw std::invalid_argument("Не удалось создать фильтр: " +
                                        descriptor.name);

        pipeline.addFilter(filter);
    }

    return pipeline;
}

void CmdLineArgs2PipelineConverter::addFilterProducer(
    const std::string& filterName, FilterProducer producer)
{
    _producers[filterName] = producer;
}

FilterProducer CmdLineArgs2PipelineConverter::getFilterProducer(
    const std::string& filterName) const
{
    auto it = _producers.find(filterName);
    if(it == _producers.end())
        return nullptr;
    return it->second;
}
