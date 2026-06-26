#include <catch2/catch_shim.hpp>

#include "core/pipeline.h"
#include "filters/transform_filters.h"

#include <utility>

TEST_CASE("Pipeline: пустой пайплайн не меняет сигнал")
{
    Pipeline pipeline;
    Waveform wave(std::vector<int16_t>{1, 2, 3});
    REQUIRE(pipeline.apply(&wave) == State::ok);
    REQUIRE(wave.samples() == std::vector<int16_t>{1, 2, 3});
    REQUIRE(pipeline.getFilterNumbers() == 0);
}

TEST_CASE("Pipeline: addFilter увеличивает счётчик и доступен по индексу")
{
    Pipeline pipeline;
    IFilter* first = pipeline.addFilter(new AmplFilter(2.0));
    pipeline.addFilter(new AmplFilter(3.0));

    REQUIRE(pipeline.getFilterNumbers() == 2);
    REQUIRE(pipeline[0] == first);
}

TEST_CASE("Pipeline: перемещение передаёт владение фильтрами")
{
    Pipeline source;
    source.addFilter(new AmplFilter(2.0));

    Pipeline destination(std::move(source));
    REQUIRE(destination.getFilterNumbers() == 1);
    REQUIRE(source.getFilterNumbers() == 0);

    Waveform wave(std::vector<int16_t>{5});
    REQUIRE(destination.apply(&wave) == State::ok);
    REQUIRE(wave.samples() == std::vector<int16_t>{10});
}

TEST_CASE("Pipeline: перемещающее присваивание")
{
    Pipeline source;
    source.addFilter(new AmplFilter(4.0));

    Pipeline destination;
    destination.addFilter(new AmplFilter(2.0));
    destination = std::move(source);

    REQUIRE(destination.getFilterNumbers() == 1);
    Waveform wave(std::vector<int16_t>{3});
    destination.apply(&wave);
    REQUIRE(wave.samples() == std::vector<int16_t>{12});
}
