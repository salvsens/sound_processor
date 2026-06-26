#ifndef SOUND_PROCESSOR_TRANSFORM_FILTERS_H
#define SOUND_PROCESSOR_TRANSFORM_FILTERS_H

#include "core/filter.h"

#include <stdexcept>

/**
 * @brief Единица измерения времени для параметров фильтров.
 */
enum class TimeUnit
{
    seconds,     /**< Секунды. */
    milliseconds /**< Миллисекунды. */
};

/**
 * @brief Умножает каждый отсчёт сигнала на заданный коэффициент.
 *
 * Формула: s'[i] = clamp(s[i] * factor). При factor > 1 часть отсчётов
 * может выйти за диапазон int16_t и ограничивается операцией clamp.
 */
class AmplFilter: public IFilter
{
public:
    /**
     * @brief Создаёт фильтр изменения амплитуды.
     * @param factor Коэффициент изменения амплитуды (factor >= 0).
     */
    explicit AmplFilter(double factor): _factor(factor)
    {
        if(factor < 0)
            throw std::invalid_argument("AmplFilter: должно быть factor >= 0");
    }

    State apply(Waveform* sound) override;

    /** @brief Возвращает коэффициент изменения амплитуды. */
    double getFactor() const { return _factor; }

private:
    double _factor; /**< Коэффициент изменения амплитуды. */
};

/**
 * @brief Нормализует сигнал к заданной доле от предельного значения int16_t.
 *
 * Находит текущий максимальный по модулю отсчёт и масштабирует сигнал так,
 * чтобы пик стал равен peak * 32767. Пустой сигнал и сигнал из одних нулей
 * остаются без изменений.
 */
class NormalizeFilter: public IFilter
{
public:
    /**
     * @brief Создаёт фильтр нормализации.
     * @param peak Целевой пик как доля от 32767 (0 <= peak <= 1, по умолчанию
     * 1).
     */
    explicit NormalizeFilter(double peak = 1.0): _peak(peak)
    {
        if(peak < 0 || peak > 1)
            throw std::invalid_argument(
                "NormalizeFilter: должно быть 0 <= peak <= 1");
    }

    State apply(Waveform* sound) override;

    /** @brief Возвращает целевой пик нормализации. */
    double getPeak() const { return _peak; }

private:
    double _peak; /**< Целевой пик как доля от 32767. */
};

/**
 * @brief Вставляет участок тишины в сигнал.
 *
 * Отсчёты, стоявшие после точки вставки, сдвигаются вправо. Если точка
 * вставки приходится за концом сигнала, тишина дописывается в его конец.
 */
class SilenceFilter: public IFilter
{
public:
    /**
     * @brief Создаёт фильтр вставки тишины.
     * @param unit Единица измерения времени для start и end.
     * @param start Момент начала тишины от начала сигнала (start >= 0).
     * @param end Момент окончания тишины, включительно (end >= start).
     */
    SilenceFilter(TimeUnit unit, double start, double end)
        : _unit(unit), _start(start), _end(end)
    {
        if(start < 0)
            throw std::invalid_argument(
                "SilenceFilter: должно быть start >= 0");
        if(end < start)
            throw std::invalid_argument(
                "SilenceFilter: должно быть end >= start");
    }

    State apply(Waveform* sound) override;

private:
    TimeUnit _unit; /**< Единица измерения времени. */
    double _start;  /**< Момент начала тишины. */
    double _end; /**< Момент окончания тишины (включительно). */
};

/**
 * @brief Изменяет длительность сигнала в заданное число раз.
 *
 * Новая длина: newSize = round(oldSize * factor). Значения нового сигнала
 * получаются линейной интерполяцией соседних исходных отсчётов:
 * s'[i] = s[l]*(1 - frac) + s[l+1]*frac, где pos = i / factor,
 * l = floor(pos), frac = pos - l. У последнего отсчёта правого соседа нет,
 * поэтому берётся s[l].
 */
class TimestretchFilter: public IFilter
{
public:
    /**
     * @brief Создаёт фильтр изменения длительности.
     * @param factor Коэффициент изменения длительности (factor > 0).
     */
    explicit TimestretchFilter(double factor): _factor(factor)
    {
        if(factor <= 0)
            throw std::invalid_argument(
                "TimestretchFilter: должно быть factor > 0");
    }

    State apply(Waveform* sound) override;

    /** @brief Возвращает коэффициент изменения длительности. */
    double getFactor() const { return _factor; }

private:
    double _factor; /**< Коэффициент изменения длительности. */
};

/**
 * @brief Сглаживает сигнал скользящим средним по окну фиксированного размера.
 *
 * Для каждого отсчёта берётся среднее по окну длины window_size с центром
 * в этом отсчёте. На краях индексы, выходящие за границы, заменяются
 * крайними значениями сигнала.
 */
class LowpassFilter: public IFilter
{
public:
    /**
     * @brief Создаёт фильтр нижних частот (скользящее среднее).
     * @param windowSize Размер окна усреднения; нечётный, >= 1.
     */
    explicit LowpassFilter(int windowSize): _windowSize(windowSize)
    {
        if(windowSize < 1)
            throw std::invalid_argument(
                "LowpassFilter: должно быть window_size >= 1");
        if(windowSize % 2 == 0)
            throw std::invalid_argument(
                "LowpassFilter: window_size должен быть нечётным");
    }

    State apply(Waveform* sound) override;

    /** @brief Возвращает размер окна усреднения. */
    int getWindowSize() const { return _windowSize; }

private:
    int _windowSize; /**< Размер окна усреднения в отсчётах. */
};

#endif  // SOUND_PROCESSOR_TRANSFORM_FILTERS_H
