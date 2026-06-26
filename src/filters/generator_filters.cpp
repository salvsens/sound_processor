#include "generator_filters.h"

#include <vector>

namespace {

constexpr double K_PI = 3.14159265358979323846;
constexpr double K_TWO_PI = 2.0 * K_PI;

}  // namespace

State AbstractGeneratorFilter::apply(Waveform* sound)
{
    if(!sound)
        return State::error;

    const auto NUM_SAMPLES =
        static_cast<size_t>(_durationMs * Waveform::K_SAMPLE_RATE / 1000.0);

    std::vector<int16_t> samples(NUM_SAMPLES);
    for(size_t i = 0; i < NUM_SAMPLES; ++i)
        samples[i] = clampToInt16(sampleValue(i));

    sound->samples() = std::move(samples);
    return State::ok;
}

double SinGenFilter::sampleValue(size_t ind) const
{
    const double TIME = timeAt(ind);
    return K_SAMPLE_MAX * std::sin(K_TWO_PI * _freqHz * TIME);
}

double AmGenFilter::sampleValue(size_t ind) const
{
    const double TIME = timeAt(ind);
    const double ENVELOPE =
        1.0 + _depth * std::sin(K_TWO_PI * _modulationHz * TIME);
    const double CARRIER = std::sin(K_TWO_PI * _carrierHz * TIME);
    return _amplitude * K_SAMPLE_MAX * ENVELOPE * CARRIER;
}

double FmGenFilter::sampleValue(size_t ind) const
{
    const double TIME = timeAt(ind);
    const double PHASE = K_TWO_PI * _carrierHz * TIME +
                         (_deviationHz / _modulationHz) *
                             std::sin(K_TWO_PI * _modulationHz * TIME);
    return _amplitude * K_SAMPLE_MAX * std::sin(PHASE);
}
