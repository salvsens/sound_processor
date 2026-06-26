#include <catch2/catch_shim.hpp>

#include "filters/generator_filters.h"

TEST_CASE("SinGenFilter: число отсчётов соответствует длительности")
{
    Waveform wave;
    SinGenFilter(440.0, 1000.0).apply(&wave);
    REQUIRE(wave.numSamples() == 44100);
}

TEST_CASE("SinGenFilter: нулевая частота даёт тишину")
{
    Waveform wave;
    SinGenFilter(0.0, 10.0).apply(&wave);
    REQUIRE_FALSE(wave.empty());
    for(int16_t sample: wave.samples())
        REQUIRE(sample == 0);
}

TEST_CASE("Генератор игнорирует и заменяет входной сигнал")
{
    Waveform wave(std::vector<int16_t>{111, 222, 333});
    SinGenFilter(0.0, 10.0).apply(&wave);
    // Прежнее содержимое полностью заменено сгенерированным.
    REQUIRE(wave.numSamples() == 441);
    REQUIRE(wave.samples()[0] == 0);
}

TEST_CASE("SinGenFilter: первый отсчёт синусоиды равен нулю")
{
    Waveform wave;
    SinGenFilter(440.0, 10.0).apply(&wave);
    REQUIRE(wave.samples().front() == 0);
}

TEST_CASE("AmGenFilter: при depth=0 вырождается в синусоиду")
{
    Waveform amplMod;
    AmGenFilter(1.0, 440.0, 5.0, 0.0, 10.0).apply(&amplMod);

    Waveform sine;
    SinGenFilter(440.0, 10.0).apply(&sine);

    REQUIRE(amplMod.numSamples() == sine.numSamples());
    REQUIRE(amplMod.samples() == sine.samples());
}

TEST_CASE("FmGenFilter: число отсчётов и диапазон значений")
{
    Waveform wave;
    REQUIRE(FmGenFilter(1.0, 440.0, 5.0, 50.0, 10.0).apply(&wave) == State::ok);
    REQUIRE(wave.numSamples() == 441);
}

TEST_CASE("Генератор толерантен к nullptr")
{
    REQUIRE(SinGenFilter(440.0, 10.0).apply(nullptr) == State::error);
}
