#include <catch2/catch_shim.hpp>

#include "filters/transform_filters.h"

#include <stdexcept>

TEST_CASE("clampToInt16: округление и ограничение диапазона")
{
    REQUIRE(clampToInt16(100.4) == 100);
    REQUIRE(clampToInt16(100.5) == 101);
    REQUIRE(clampToInt16(40000.0) == 32767);
    REQUIRE(clampToInt16(-40000.0) == -32768);
}

TEST_CASE("AmplFilter: умножение отсчётов и clamp")
{
    SECTION("обычное усиление")
    {
        Waveform wave(std::vector<int16_t>{100, -100, 50});
        AmplFilter filter(2.0);
        REQUIRE(filter.apply(&wave) == State::ok);
        REQUIRE(wave.samples() == std::vector<int16_t>{200, -200, 100});
    }

    SECTION("выход за диапазон ограничивается clamp")
    {
        Waveform wave(std::vector<int16_t>{20000, -20000});
        AmplFilter filter(2.0);
        filter.apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{32767, -32768});
    }

    SECTION("factor = 0 заглушает сигнал")
    {
        Waveform wave(std::vector<int16_t>{123, -456});
        AmplFilter(0.0).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{0, 0});
    }
}

TEST_CASE("AmplFilter: отрицательный factor отвергается конструктором")
{
    REQUIRE_THROWS_AS(AmplFilter(-1.0), std::invalid_argument);
}

TEST_CASE("NormalizeFilter: пик масштабируется к цели")
{
    SECTION("масштабирование до полного пика")
    {
        Waveform wave(std::vector<int16_t>{100, -50});
        NormalizeFilter(1.0).apply(&wave);
        REQUIRE(wave.samples()[0] == 32767);
    }

    SECTION("пустой сигнал не меняется")
    {
        Waveform wave;
        REQUIRE(NormalizeFilter(1.0).apply(&wave) == State::ok);
        REQUIRE(wave.empty());
    }

    SECTION("сигнал из нулей не меняется")
    {
        Waveform wave(std::vector<int16_t>{0, 0, 0});
        NormalizeFilter(1.0).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{0, 0, 0});
    }
}

TEST_CASE("NormalizeFilter: peak вне [0,1] отвергается")
{
    REQUIRE_THROWS_AS(NormalizeFilter(1.5), std::invalid_argument);
    REQUIRE_THROWS_AS(NormalizeFilter(-0.1), std::invalid_argument);
}

TEST_CASE("SilenceFilter: вставка со сдвигом вправо")
{
    SECTION("вставка в начало")
    {
        Waveform wave(std::vector<int16_t>{1, 2, 3, 4});
        SilenceFilter(TimeUnit::seconds, 0.0, 0.0).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{0, 1, 2, 3, 4});
    }

    SECTION("точка вставки за концом")
    {
        Waveform wave(std::vector<int16_t>{1, 2});
        SilenceFilter(TimeUnit::seconds, 100.0, 100.0).apply(&wave);
        REQUIRE(wave.numSamples() == 3);
        REQUIRE(wave.samples().back() == 0);
    }
}

TEST_CASE("SilenceFilter: end < start отвергается")
{
    REQUIRE_THROWS_AS(SilenceFilter(TimeUnit::seconds, 0.5, 0.2),
                      std::invalid_argument);
}

TEST_CASE("TimestretchFilter: линейная интерполяция")
{
    SECTION("растяжение в 2 раза")
    {
        Waveform wave(std::vector<int16_t>{0, 100});
        TimestretchFilter(2.0).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{0, 50, 100, 100});
    }

    SECTION("сжатие в 2 раза")
    {
        Waveform wave(std::vector<int16_t>{0, 100});
        TimestretchFilter(0.5).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{0});
    }
}

TEST_CASE("TimestretchFilter: factor <= 0 отвергается")
{
    REQUIRE_THROWS_AS(TimestretchFilter(0.0), std::invalid_argument);
}

TEST_CASE("LowpassFilter: скользящее среднее с краями")
{
    SECTION("окно 3 усредняет с повтором краёв")
    {
        Waveform wave(std::vector<int16_t>{0, 30, 0});
        LowpassFilter(3).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{10, 10, 10});
    }

    SECTION("окно 1 не меняет сигнал")
    {
        Waveform wave(std::vector<int16_t>{5, 10, 15});
        LowpassFilter(1).apply(&wave);
        REQUIRE(wave.samples() == std::vector<int16_t>{5, 10, 15});
    }
}

TEST_CASE("LowpassFilter: чётное или нулевое окно отвергается")
{
    REQUIRE_THROWS_AS(LowpassFilter(2), std::invalid_argument);
    REQUIRE_THROWS_AS(LowpassFilter(0), std::invalid_argument);
}

TEST_CASE("Преобразующий фильтр толерантен к nullptr")
{
    REQUIRE(AmplFilter(1.0).apply(nullptr) == State::error);
}
