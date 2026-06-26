#include "pipeline.h"

Pipeline::Pipeline(Pipeline&& other) noexcept
    : _filters(std::move(other._filters))
{
    other._filters.clear();
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
{
    if(this != &other)
    {
        clear();
        _filters = std::move(other._filters);
        other._filters.clear();
    }
    return *this;
}

Pipeline::~Pipeline() { clear(); }

void Pipeline::clear()
{
    for(IFilter* filter: _filters)
        delete filter;
    _filters.clear();
}

State Pipeline::apply(Waveform* sound) const
{
    for(IFilter* filter: _filters)
        if(filter->apply(sound) != State::ok)
            return State::error;
    return State::ok;
}

IFilter* Pipeline::addFilter(IFilter* filter)
{
    if(!filter)
        return nullptr;
    _filters.push_back(filter);
    return filter;
}
