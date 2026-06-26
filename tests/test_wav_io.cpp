#include <catch2/catch_shim.hpp>

#include "core/wav_io.h"

#include <filesystem>
#include <fstream>

namespace {

/** @brief Возвращает уникальный временный путь для тестового WAV-файла. */
std::string tempWavPath(const char* tag)
{
    auto path = std::filesystem::temp_directory_path() /
                ("sp_test_" + std::string(tag) + ".wav");
    return path.string();
}

}  // namespace

TEST_CASE("WavIo: round-trip сохраняет отсчёты")
{
    const std::string PATH = tempWavPath("roundtrip");
    Waveform original(std::vector<int16_t>{0, 1642, -3279, 26215, -26215});

    Wav::WavWriter writer;
    writer.write(PATH, original);

    Wav::WavReader reader;
    Waveform loaded = reader.read(PATH);

    REQUIRE(loaded.numSamples() == original.numSamples());
    REQUIRE(loaded.samples() == original.samples());

    std::filesystem::remove(PATH);
}

TEST_CASE("WavIo: пустой сигнал даёт валидный файл из 44 байт")
{
    const std::string PATH = tempWavPath("empty");
    Waveform empty;

    Wav::WavWriter writer;
    writer.write(PATH, empty);

    REQUIRE(std::filesystem::file_size(PATH) == 44);

    Wav::WavReader reader;
    Waveform loaded = reader.read(PATH);
    REQUIRE(loaded.empty());

    std::filesystem::remove(PATH);
}

TEST_CASE("WavIo: чтение несуществующего файла бросает исключение")
{
    Wav::WavReader reader;
    REQUIRE_THROWS_AS(reader.read("/no/such/file_sp.wav"), std::exception);
}

TEST_CASE("WavIo: чтение файла неверного формата бросает исключение")
{
    const std::string PATH = tempWavPath("garbage");
    {
        std::ofstream file(PATH, std::ios::binary);
        file << "this is definitely not a wav file";
    }

    Wav::WavReader reader;
    REQUIRE_THROWS_AS(reader.read(PATH), std::exception);

    std::filesystem::remove(PATH);
}
