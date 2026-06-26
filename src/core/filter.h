#ifndef SOUND_PROCESSOR_FILTER_H
#define SOUND_PROCESSOR_FILTER_H

#include "waveform.h"

#include <cmath>
#include <limits>

/**
 * @brief Результат применения фильтра.
 */
enum class State
{
    ok,   /**< Фильтр успешно применён. */
    error /**< При применении фильтра произошла ошибка. */
};

/** @brief Множитель амплитуды по спецификации (максимум знакового 16-бит). */
static constexpr double K_SAMPLE_MAX = 32767.0;

/**
 * @brief Ограничивает вещественное значение диапазоном int16_t с округлением.
 * @param value Значение отсчёта до квантования.
 * @return Округлённое значение, прижатое к границам [-32768, 32767].
 */
inline int16_t clampToInt16(double value)
{
    constexpr double LOW = std::numeric_limits<int16_t>::min();
    constexpr double HIGH = std::numeric_limits<int16_t>::max();
    double rounded = std::round(value);
    if(rounded > HIGH)
        rounded = HIGH;
    if(rounded < LOW)
        rounded = LOW;
    return static_cast<int16_t>(rounded);
}

/**
 * @brief Абстрактный интерфейс звукового фильтра.
 */
class IFilter
{
public:
    virtual ~IFilter() = default;

    /**
     * @brief Применяет фильтр к звуковому фрагменту, изменяя его на месте.
     * @param sound Указатель на обрабатываемый фрагмент.
     * @return State::ok при успехе, иначе State::error.
     */
    virtual State apply(Waveform* sound) = 0;
};

#endif  // SOUND_PROCESSOR_FILTER_H
