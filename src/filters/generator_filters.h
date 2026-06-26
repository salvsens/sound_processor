#ifndef SOUND_PROCESSOR_GENERATOR_FILTERS_H
#define SOUND_PROCESSOR_GENERATOR_FILTERS_H

#include "core/filter.h"

#include <stdexcept>

/**
 * @brief Общий базовый класс для фильтров-генераторов.
 *
 * Генераторы толерантны к пустому фрагменту и формируют выходной целиком
 * по своим параметрам. Метод apply() реализует общий шаблон: вычисляет число
 * отсчётов по длительности, заполняет буфер значениями виртуального метода
 * sampleValue() и записывает их в фрагмент.
 */
class AbstractGeneratorFilter: public IFilter
{
public:
    /**
     * @brief Создаёт генератор заданной длительности.
     * @param durationMs Длительность генерируемого сигнала, мс (>= 0).
     */
    explicit AbstractGeneratorFilter(double durationMs): _durationMs(durationMs)
    {
        if(durationMs < 0)
            throw std::invalid_argument(
                "AbstractGeneratorFilter: должно быть duration_ms >= 0");
    }

    State apply(Waveform* sound) override;

protected:
    /**
     * @brief Вычисляет значение отсчёта с индексом @p ind до квантования.
     * @param ind Индекс отсчёта.
     * @return Значение отсчёта в шкале int16_t (ещё не ограниченное clamp).
     */
    virtual double sampleValue(size_t ind) const = 0;

    /** @brief Возвращает время отсчёта с индексом @p ind в секундах
     * (t = i / 44100). */
    static double timeAt(size_t ind)
    {
        return static_cast<double>(ind) / Waveform::K_SAMPLE_RATE;
    }

    double _durationMs; /**< Длительность сигнала, мс. */
};

/**
 * @brief Генератор синусоиды заданной частоты и длительности.
 *
 * Формула: s[i] = clamp(32767 * sin(2*pi*frequency_hz*t)).
 */
class SinGenFilter: public AbstractGeneratorFilter
{
public:
    /**
     * @brief Создаёт генератор синусоиды.
     * @param freqHz Частота синусоиды, Гц (>= 0).
     * @param durationMs Длительность сигнала, мс (>= 0).
     */
    SinGenFilter(double freqHz, double durationMs)
        : AbstractGeneratorFilter(durationMs), _freqHz(freqHz)
    {
        if(freqHz < 0)
            throw std::invalid_argument(
                "SinGenFilter: должно быть frequency_hz >= 0");
    }

protected:
    double sampleValue(size_t ind) const override;

private:
    double _freqHz; /**< Частота синусоиды, Гц. */
};

/**
 * @brief Генератор амплитудно-модулированного сигнала - тремоло.
 *
 * Несущая синусоида, амплитуда которой меняется по закону модулирующей:
 * envelope = 1 + depth*sin(2*pi*modulation_hz*t),
 * carrier  = sin(2*pi*carrier_hz*t),
 * s[i] = clamp(amplitude * 32767 * envelope * carrier).
 */
class AmGenFilter: public AbstractGeneratorFilter
{
public:
    /**
     * @brief Создаёт генератор АМ-сигнала.
     * @param amplitude Общая амплитуда (0 <= amplitude <= 1).
     * @param carrierHz Частота несущей, Гц (>= 0).
     * @param modulationHz Частота модулирующей синусоиды, Гц (>= 0).
     * @param depth Глубина модуляции (0 <= depth <= 1).
     * @param durationMs Длительность сигнала, мс (>= 0).
     */
    AmGenFilter(double amplitude, double carrierHz, double modulationHz,
                double depth, double durationMs)
        : AbstractGeneratorFilter(durationMs), _amplitude(amplitude),
          _carrierHz(carrierHz), _modulationHz(modulationHz), _depth(depth)
    {
        if(amplitude < 0 || amplitude > 1)
            throw std::invalid_argument(
                "AmGenFilter: должно быть 0 <= amplitude <= 1");
        if(carrierHz < 0)
            throw std::invalid_argument(
                "AmGenFilter: должно быть carrier_hz >= 0");
        if(modulationHz < 0)
            throw std::invalid_argument(
                "AmGenFilter: должно быть modulation_hz >= 0");
        if(depth < 0 || depth > 1)
            throw std::invalid_argument(
                "AmGenFilter: должно быть 0 <= depth <= 1");
    }

protected:
    double sampleValue(size_t ind) const override;

private:
    double _amplitude; /**< Общая амплитуда сигнала. */
    double _carrierHz; /**< Частота несущей, Гц. */
    double _modulationHz; /**< Частота модулирующей синусоиды, Гц. */
    double _depth; /**< Глубина модуляции. */
};

/**
 * @brief Генератор частотно-модулированного сигнала - вибрато.
 *
 * Несущая синусоида, мгновенная частота которой отклоняется по закону
 * модулирующей:
 * phase = 2*pi*carrier_hz*t +
 * (deviation_hz/modulation_hz)*sin(2*pi*modulation_hz*t),
 * s[i] = clamp(amplitude * 32767 * sin(phase)).
 */
class FmGenFilter: public AbstractGeneratorFilter
{
public:
    /**
     * @brief Создаёт генератор ЧМ-сигнала.
     * @param amplitude Амплитуда сигнала (0 <= amplitude <= 1).
     * @param carrierHz Центральная частота несущей, Гц (>= 0).
     * @param modulationHz Частота модулирующей синусоиды, Гц (> 0).
     * @param deviationHz Максимальное отклонение частоты, Гц (>= 0).
     * @param durationMs Длительность сигнала, мс (>= 0).
     */
    FmGenFilter(double amplitude, double carrierHz, double modulationHz,
                double deviationHz, double durationMs)
        : AbstractGeneratorFilter(durationMs), _amplitude(amplitude),
          _carrierHz(carrierHz), _modulationHz(modulationHz),
          _deviationHz(deviationHz)
    {
        if(amplitude < 0 || amplitude > 1)
            throw std::invalid_argument(
                "FmGenFilter: должно быть 0 <= amplitude <= 1");
        if(carrierHz < 0)
            throw std::invalid_argument(
                "FmGenFilter: должно быть carrier_hz >= 0");
        if(modulationHz <= 0)
            throw std::invalid_argument(
                "FmGenFilter: должно быть modulation_hz > 0");
        if(deviationHz < 0)
            throw std::invalid_argument(
                "FmGenFilter: должно быть deviation_hz >= 0");
    }

protected:
    double sampleValue(size_t ind) const override;

private:
    double _amplitude; /**< Амплитуда сигнала. */
    double _carrierHz; /**< Центральная частота несущей, Гц. */
    double _modulationHz; /**< Частота модулирующей синусоиды, Гц. */
    double _deviationHz; /**< Максимальное отклонение частоты, Гц. */
};

#endif  // SOUND_PROCESSOR_GENERATOR_FILTERS_H
