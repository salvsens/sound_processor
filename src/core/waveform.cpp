#include "waveform.h"

Waveform::Waveform(std::vector<int16_t> samples): _samples(std::move(samples))
{
}

size_t Waveform::msToSamples(double msec) const
{
    return static_cast<size_t>(msec * K_SAMPLE_RATE / 1000.0);
}

size_t Waveform::secToSamples(double sec) const
{
    return static_cast<size_t>(sec * K_SAMPLE_RATE);
}

double Waveform::samplesToMs(size_t n) const
{
    return static_cast<double>(n) * 1000.0 / K_SAMPLE_RATE;
}

double Waveform::samplesToSec(size_t n) const
{
    return static_cast<double>(n) / K_SAMPLE_RATE;
}

double Waveform::durationMs() const { return samplesToMs(_samples.size()); }

double Waveform::durationSec() const { return samplesToSec(_samples.size()); }
