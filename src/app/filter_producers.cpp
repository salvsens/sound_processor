#include "filter_producers.h"

#include "filters/generator_filters.h"
#include "filters/transform_filters.h"

#include <stdexcept>
#include <string>

namespace {

/** @brief Разбирает токен как double, требуя полного потребления строки. */
double parseDouble(const std::string& token, const char* what)
{
    size_t pos = 0;
    double value = 0.0;
    try
    {
        value = std::stod(token, &pos);
    }
    catch(const std::exception&)
    {
        throw std::invalid_argument(
            std::string("Некорректное число для параметра ") + what + ": '" +
            token + "'");
    }
    if(pos != token.size())
        throw std::invalid_argument(
            std::string("Некорректное число для параметра ") + what + ": '" +
            token + "'");
    return value;
}

/** @brief Разбирает токен как int, требуя полного потребления строки. */
int parseInt(const std::string& token, const char* what)
{
    size_t pos = 0;
    int value = 0;
    try
    {
        value = std::stoi(token, &pos);
    }
    catch(const std::exception&)
    {
        throw std::invalid_argument(
            std::string("Некорректное целое для параметра ") + what + ": '" +
            token + "'");
    }
    if(pos != token.size())
        throw std::invalid_argument(
            std::string("Некорректное целое для параметра ") + what + ": '" +
            token + "'");
    return value;
}

/** @brief Проверяет, что параметров не меньше требуемого числа. */
void requireParams(const FilterDescriptor& filterDescriptor, size_t count,
                   const char* filterName)
{
    if(filterDescriptor.params.size() < count)
        throw std::invalid_argument(
            std::string("Недостаточно параметров для фильтра ") + filterName);
}

/** @brief Преобразует строку единицы времени в TimeUnit. */
TimeUnit parseTimeUnit(const std::string& token)
{
    if(token == "sec")
        return TimeUnit::seconds;
    if(token == "ms")
        return TimeUnit::milliseconds;
    throw std::invalid_argument(
        "Некорректная единица времени (ожидается sec или ms): '" + token + "'");
}

}  // namespace

namespace FilterProducers {

IFilter* amplFilterCreator(const FilterDescriptor& filterDescriptor)
{
    requireParams(filterDescriptor, 1, "ampl");
    const double FACTOR = parseDouble(filterDescriptor.params[0], "factor");
    return new AmplFilter(FACTOR);
}

IFilter* normalizeFilterCreator(const FilterDescriptor& filterDescriptor)
{
    if(filterDescriptor.params.empty())
        return new NormalizeFilter();
    const double PEAK = parseDouble(filterDescriptor.params[0], "peak");
    return new NormalizeFilter(PEAK);
}

IFilter* silenceFilterCreator(const FilterDescriptor& filterDescriptor)
{
    requireParams(filterDescriptor, 3, "silence");
    const TimeUnit UNIT = parseTimeUnit(filterDescriptor.params[0]);
    const double START = parseDouble(filterDescriptor.params[1], "start");
    const double END = parseDouble(filterDescriptor.params[2], "end");
    return new SilenceFilter(UNIT, START, END);
}

IFilter* timestretchFilterCreator(const FilterDescriptor& filterDescriptor)
{
    requireParams(filterDescriptor, 1, "timestretch");
    const double FACTOR = parseDouble(filterDescriptor.params[0], "factor");
    return new TimestretchFilter(FACTOR);
}

IFilter* lowpassFilterCreator(const FilterDescriptor& filterDescriptor)
{
    requireParams(filterDescriptor, 1, "lowpass");
    const int WINDOW_SIZE = parseInt(filterDescriptor.params[0], "window_size");
    return new LowpassFilter(WINDOW_SIZE);
}

IFilter* generatorFilterCreator(const FilterDescriptor& filterDescriptor)
{
    requireParams(filterDescriptor, 1, "generator");
    const std::string& kind = filterDescriptor.params[0];

    if(kind == "sin")
    {
        requireParams(filterDescriptor, 3, "generator sin");
        const double FREQ =
            parseDouble(filterDescriptor.params[1], "frequency_hz");
        const double DURATION =
            parseDouble(filterDescriptor.params[2], "duration_ms");
        return new SinGenFilter(FREQ, DURATION);
    }
    if(kind == "am")
    {
        requireParams(filterDescriptor, 6, "generator am");
        const double AMPLITUDE =
            parseDouble(filterDescriptor.params[1], "amplitude");
        const double CARRIER =
            parseDouble(filterDescriptor.params[2], "carrier_hz");
        const double MODULATION =
            parseDouble(filterDescriptor.params[3], "modulation_hz");
        const double DEPTH = parseDouble(filterDescriptor.params[4], "depth");
        const double DURATION =
            parseDouble(filterDescriptor.params[5], "duration_ms");
        return new AmGenFilter(AMPLITUDE, CARRIER, MODULATION, DEPTH, DURATION);
    }
    if(kind == "fm")
    {
        requireParams(filterDescriptor, 6, "generator fm");
        const double AMPLITUDE =
            parseDouble(filterDescriptor.params[1], "amplitude");
        const double CARRIER =
            parseDouble(filterDescriptor.params[2], "carrier_hz");
        const double MODULATION =
            parseDouble(filterDescriptor.params[3], "modulation_hz");
        const double DEVIATION =
            parseDouble(filterDescriptor.params[4], "deviation_hz");
        const double DURATION =
            parseDouble(filterDescriptor.params[5], "duration_ms");
        return new FmGenFilter(AMPLITUDE, CARRIER, MODULATION, DEVIATION,
                               DURATION);
    }

    throw std::invalid_argument("Неизвестная разновидность генератора: '" +
                                kind + "'");
}

}  // namespace FilterProducers
