#include "transform_filters.h"

#include <algorithm>
#include <vector>

namespace {

/**
 * @brief Переводит момент времени в индекс отсчёта с учётом единицы измерения.
 */
size_t timeToSamples(double value, TimeUnit unit)
{
    constexpr double RATE = Waveform::K_SAMPLE_RATE;
    return unit == TimeUnit::seconds
               ? static_cast<size_t>(value * RATE)
               : static_cast<size_t>(value * RATE / 1000.0);
}

}  // namespace

State AmplFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;
    for(int16_t& sample: sound->samples())
        sample = clampToInt16(static_cast<double>(sample) * _factor);
    return State::ok;
}

State NormalizeFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;
    auto& samples = sound->samples();
    if(samples.empty())
        return State::ok;

    int currentPeak = 0;
    for(int16_t sample: samples)
    {
        int magnitude = std::abs(static_cast<int>(sample));
        if(magnitude > currentPeak)
            currentPeak = magnitude;
    }
    if(currentPeak == 0)
        return State::ok;

    double scale = _peak * K_SAMPLE_MAX / currentPeak;
    for(int16_t& sample: samples)
        sample = clampToInt16(static_cast<double>(sample) * scale);
    return State::ok;
}

State SilenceFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;
    auto& samples = sound->samples();

    const size_t START_IDX = timeToSamples(_start, _unit);
    const size_t END_IDX = timeToSamples(_end, _unit);
    const size_t SILENCE_LEN = END_IDX - START_IDX + 1;

    // Если точка вставки за концом сигнала — дописываем тишину в конец.
    const size_t INSERT_POS = std::min(START_IDX, samples.size());
    samples.insert(samples.begin() + static_cast<std::ptrdiff_t>(INSERT_POS),
                   SILENCE_LEN, static_cast<int16_t>(0));
    return State::ok;
}

State TimestretchFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;
    auto& samples = sound->samples();
    const size_t OLD_SIZE = samples.size();
    const auto NEW_SIZE = static_cast<size_t>(
        std::llround(static_cast<double>(OLD_SIZE) * _factor));

    if(OLD_SIZE == 0 || NEW_SIZE == 0)
    {
        samples.clear();
        return State::ok;
    }

    std::vector<int16_t> result(NEW_SIZE);
    for(size_t i = 0; i < NEW_SIZE; ++i)
    {
        const double POS = static_cast<double>(i) / _factor;
        auto left = static_cast<size_t>(std::floor(POS));
        if(left >= OLD_SIZE)
            left = OLD_SIZE - 1;
        const double FRAC = POS - static_cast<double>(left);

        double value;
        if(left + 1 < OLD_SIZE)
            value = samples[left] * (1.0 - FRAC) + samples[left + 1] * FRAC;
        else
            value = samples[left];
        result[i] = clampToInt16(value);
    }

    samples = std::move(result);
    return State::ok;
}

State LowpassFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;
    auto& samples = sound->samples();
    if(samples.empty() || _windowSize <= 1)
        return State::ok;

    const int SIZE = static_cast<int>(samples.size());
    const int HALF = _windowSize / 2;
    std::vector<int16_t> result(samples.size());

    for(int i = 0; i < SIZE; ++i)
    {
        long sum = 0;
        for(int k = -HALF; k <= HALF; ++k)
        {
            int idx = i + k;
            if(idx < 0)
                idx = 0;
            if(idx >= SIZE)
                idx = SIZE - 1;
            sum += samples[idx];
        }
        result[i] = clampToInt16(static_cast<double>(sum) / _windowSize);
    }

    samples = std::move(result);
    return State::ok;
}
