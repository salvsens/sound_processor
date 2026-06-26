#ifndef SOUND_PROCESSOR_FILTER_PRODUCERS_H
#define SOUND_PROCESSOR_FILTER_PRODUCERS_H

#include "core/filter.h"
#include "core/filter_descriptor.h"

/**
 * @brief Стандартные продюсеры фильтров.
 *
 * Каждая функция интерпретирует параметры дескриптора и создаёт фильтр
 * соответствующего типа в куче, возвращая его как IFilter*. При некорректном
 * числе или значениях параметров бросается std::invalid_argument.
 */
namespace FilterProducers {

/** @brief Создаёт AmplFilter: params = { factor }. */
IFilter* amplFilterCreator(const FilterDescriptor& filterDescriptor);

/** @brief Создаёт NormalizeFilter: params = { [peak] }. */
IFilter* normalizeFilterCreator(const FilterDescriptor& filterDescriptor);

/** @brief Создаёт SilenceFilter: params = { unit, start, end }. */
IFilter* silenceFilterCreator(const FilterDescriptor& filterDescriptor);

/** @brief Создаёт TimestretchFilter: params = { factor }. */
IFilter* timestretchFilterCreator(const FilterDescriptor& filterDescriptor);

/** @brief Создаёт LowpassFilter: params = { window_size }. */
IFilter* lowpassFilterCreator(const FilterDescriptor& filterDescriptor);

/**
 * @brief Создаёт генератор по разновидности сигнала.
 *
 * params[0] = { sin | am | fm }, далее — параметры выбранного генератора:
 * - sin: frequency_hz duration_ms
 * - am:  amplitude carrier_hz modulation_hz depth duration_ms
 * - fm:  amplitude carrier_hz modulation_hz deviation_hz duration_ms
 */
IFilter* generatorFilterCreator(const FilterDescriptor& filterDescriptor);

}  // namespace FilterProducers

#endif  // SOUND_PROCESSOR_FILTER_PRODUCERS_H
