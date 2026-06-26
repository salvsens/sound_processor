#include <catch2/catch_shim.hpp>

#include "core/waveform.h"

TEST_CASE("Waveform: пустой по умолчанию")
{
    Waveform wave;
    REQUIRE(wave.empty());
    REQUIRE(wave.numSamples() == 0);
}

TEST_CASE("Waveform: конструктор из буфера отсчётов")
{
    Waveform wave(std::vector<int16_t>{1, 2, 3, 4});
    REQUIRE_FALSE(wave.empty());
    REQUIRE(wave.numSamples() == 4);
    REQUIRE(wave.samples()[2] == 3);
}

TEST_CASE("Waveform: перевод единиц времени")
{
    Waveform wave;

    SECTION("секунды и миллисекунды в отсчёты")
    {
        REQUIRE(wave.secToSamples(1.0) == 44100);
        REQUIRE(wave.msToSamples(1000.0) == 44100);
        REQUIRE(wave.msToSamples(0.0) == 0);
    }

    SECTION("отсчёты обратно во время")
    {
        REQUIRE(wave.samplesToSec(44100) == Catch::Approx(1.0));
        REQUIRE(wave.samplesToMs(44100) == Catch::Approx(1000.0));
    }
}

TEST_CASE("Waveform: длительность вычисляется по числу отсчётов")
{
    Waveform wave(std::vector<int16_t>(44100, 0));
    REQUIRE(wave.durationSec() == Catch::Approx(1.0));
    REQUIRE(wave.durationMs() == Catch::Approx(1000.0));
}
