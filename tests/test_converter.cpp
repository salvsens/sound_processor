#include <catch2/catch_shim.hpp>

#include "app/converter.h"
#include "app/filter_producers.h"

#include <stdexcept>

namespace {

/** @brief Создаёт конвертер со всеми стандартными продюсерами. */
CmdLineArgs2PipelineConverter makeConfiguredConverter()
{
    CmdLineArgs2PipelineConverter converter;
    converter.addFilterProducer("ampl", FilterProducers::amplFilterCreator);
    converter.addFilterProducer("silence",
                                FilterProducers::silenceFilterCreator);
    converter.addFilterProducer("generator",
                                FilterProducers::generatorFilterCreator);
    return converter;
}

}  // namespace

TEST_CASE("Converter: getFilterProducer для неизвестного имени даёт nullptr")
{
    CmdLineArgs2PipelineConverter converter;
    REQUIRE(converter.getFilterProducer("unknown") == nullptr);
}

TEST_CASE("Converter: зарегистрированный продюсер находится")
{
    CmdLineArgs2PipelineConverter converter;
    converter.addFilterProducer("ampl", FilterProducers::amplFilterCreator);
    REQUIRE(converter.getFilterProducer("ampl") != nullptr);
}

TEST_CASE("Converter: createPipeline строит пайплайн из дескрипторов")
{
    auto converter = makeConfiguredConverter();

    std::vector<FilterDescriptor> descriptors{
        {"ampl", {"0.8"}},
        {"generator", {"sin", "440", "1000"}},
    };

    Pipeline pipeline = converter.createPipeline(descriptors);
    REQUIRE(pipeline.getFilterNumbers() == 2);
}

TEST_CASE("Converter: пустой список даёт пустой пайплайн")
{
    auto converter = makeConfiguredConverter();
    Pipeline pipeline = converter.createPipeline({});
    REQUIRE(pipeline.getFilterNumbers() == 0);
}

TEST_CASE("Converter: неизвестный фильтр бросает исключение")
{
    auto converter = makeConfiguredConverter();
    std::vector<FilterDescriptor> descriptors{{"nosuch", {}}};
    REQUIRE_THROWS_AS(converter.createPipeline(descriptors), std::exception);
}

TEST_CASE("Converter: некорректные параметры фильтра бросают исключение")
{
    auto converter = makeConfiguredConverter();

    SECTION("нечисловой параметр")
    {
        std::vector<FilterDescriptor> descriptors{{"ampl", {"abc"}}};
        REQUIRE_THROWS_AS(converter.createPipeline(descriptors),
                          std::exception);
    }

    SECTION("недостаточно параметров")
    {
        std::vector<FilterDescriptor> descriptors{{"silence", {"sec"}}};
        REQUIRE_THROWS_AS(converter.createPipeline(descriptors),
                          std::exception);
    }

    SECTION("недопустимое значение (factor < 0)")
    {
        std::vector<FilterDescriptor> descriptors{{"ampl", {"-1"}}};
        REQUIRE_THROWS_AS(converter.createPipeline(descriptors),
                          std::exception);
    }

    SECTION("неизвестная разновидность генератора")
    {
        std::vector<FilterDescriptor> descriptors{
            {"generator", {"triangle", "1", "2"}}};
        REQUIRE_THROWS_AS(converter.createPipeline(descriptors),
                          std::exception);
    }
}
