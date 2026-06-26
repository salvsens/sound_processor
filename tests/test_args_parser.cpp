#include <catch2/catch_shim.hpp>

#include "app/args_parser.h"

#include <string>
#include <vector>

namespace {

/** @brief Вспомогательная обёртка для построения (argc, argv) из строк. */
class Args
{
public:
    Args(std::initializer_list<std::string> tokens): _storage(tokens)
    {
        for(std::string& token: _storage)
            _argv.push_back(token.data());
    }

    int argc() const { return static_cast<int>(_storage.size()); }
    char** argv() { return _argv.data(); }

private:
    std::vector<std::string> _storage;
    std::vector<char*> _argv;
};

}  // namespace

TEST_CASE("ArgsParser: без аргументов возвращает noArgs")
{
    Args args{"sound_processor"};
    ArgsParser parser;
    REQUIRE(parser.parse(args.argc(), args.argv()) ==
            ArgsParser::Result::noArgs);
}

TEST_CASE("ArgsParser: корректный разбор файлов и фильтра")
{
    Args args{"sound_processor", "-i", "in.wav", "-o",
              "out.wav",         "-f", "ampl",   "0.8"};
    ArgsParser parser;

    REQUIRE(parser.parse(args.argc(), args.argv()) == ArgsParser::Result::ok);
    REQUIRE(parser.hasInFileName());
    REQUIRE(parser.hasOutFileName());
    REQUIRE(parser.getInFileName() == "in.wav");
    REQUIRE(parser.getOutFileName() == "out.wav");
    REQUIRE(parser.getFilterDescriptors().size() == 1);
    REQUIRE(parser.getFilterDescriptors()[0].name == "ampl");
    REQUIRE(parser.getFilterDescriptors()[0].params ==
            std::vector<std::string>{"0.8"});
}

TEST_CASE("ArgsParser: фильтр с несколькими параметрами")
{
    Args args{"sound_processor", "-f", "silence", "sec", "0.2", "0.4"};
    ArgsParser parser;
    parser.parse(args.argc(), args.argv());

    const auto& descriptors = parser.getFilterDescriptors();
    REQUIRE(descriptors.size() == 1);
    REQUIRE(descriptors[0].name == "silence");
    REQUIRE(descriptors[0].params ==
            std::vector<std::string>{"sec", "0.2", "0.4"});
}

TEST_CASE("ArgsParser: несколько фильтров")
{
    Args args{"sound_processor", "-f", "ampl", "0.8", "-f", "timestretch", "2"};
    ArgsParser parser;
    parser.parse(args.argc(), args.argv());
    REQUIRE(parser.getFilterDescriptors().size() == 2);
}

TEST_CASE("ArgsParser: генератор без входного файла корректен")
{
    Args args{"sound_processor", "-o",  "out.wav", "-f",
              "generator",       "sin", "440",     "1000"};
    ArgsParser parser;
    REQUIRE(parser.parse(args.argc(), args.argv()) == ArgsParser::Result::ok);
    REQUIRE_FALSE(parser.hasInFileName());
}

TEST_CASE("ArgsParser: некорректные комбинации дают badArgs")
{
    ArgsParser parser;

    SECTION("дублирование -i")
    {
        Args args{"sound_processor", "-i", "a.wav", "-i", "b.wav"};
        REQUIRE(parser.parse(args.argc(), args.argv()) ==
                ArgsParser::Result::badArgs);
    }

    SECTION("-i без имени файла")
    {
        Args args{"sound_processor", "-i"};
        REQUIRE(parser.parse(args.argc(), args.argv()) ==
                ArgsParser::Result::badArgs);
    }

    SECTION("-f без названия фильтра")
    {
        Args args{"sound_processor", "-f"};
        REQUIRE(parser.parse(args.argc(), args.argv()) ==
                ArgsParser::Result::badArgs);
    }

    SECTION("неизвестный токен верхнего уровня")
    {
        Args args{"sound_processor", "garbage"};
        REQUIRE(parser.parse(args.argc(), args.argv()) ==
                ArgsParser::Result::badArgs);
    }
}

TEST_CASE("ArgsParser: повторный разбор сбрасывает состояние")
{
    ArgsParser parser;

    Args first{"sound_processor", "-i", "a.wav", "-f", "ampl", "0.8"};
    parser.parse(first.argc(), first.argv());

    Args second{"sound_processor", "-o", "b.wav"};
    parser.parse(second.argc(), second.argv());

    REQUIRE_FALSE(parser.hasInFileName());
    REQUIRE(parser.hasOutFileName());
    REQUIRE(parser.getFilterDescriptors().empty());
}
