#include "wav_io.h"

#include <fstream>
#include <stdexcept>

namespace Wav {

Waveform WavReader::read(const std::string& path) const
{
    std::ifstream file(path, std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("WavReader: не удалось открыть файл: " + path);

    // Чтение RIFF-чанка
    RiffChunk riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    if(!file)
        throw std::runtime_error(
            "WavReader: файл слишком короткий (RIFF-заголовок): " + path);
    if(riff.chunkId != ID_RIFF)
        throw std::runtime_error("WavReader: отсутствует сигнатура RIFF: " +
                                 path);
    if(riff.waveId != ID_WAVE)
        throw std::runtime_error("WavReader: отсутствует идентификатор WAVE: " +
                                 path);

    // Чтение fmt-чанка
    FmtChunk fmt{};
    file.read(reinterpret_cast<char*>(&fmt), sizeof(fmt));
    if(!file)
        throw std::runtime_error(
            "WavReader: файл слишком короткий (fmt-чанк): " + path);
    if(fmt.chunkId != ID_FMT)
        throw std::runtime_error("WavReader: отсутствует чанк fmt: " + path);
    if(fmt.wFormatTag != 1)
        throw std::runtime_error(
            "WavReader: поддерживается только PCM (wFormatTag=1): " + path);
    if(fmt.wChannels != Waveform::K_CHANNELS)
        throw std::runtime_error(
            "WavReader: поддерживается только моно (1 канал): " + path);
    if(fmt.dwSamplesPerSec != Waveform::K_SAMPLE_RATE)
        throw std::runtime_error("WavReader: поддерживается только 44100 Гц: " +
                                 path);
    if(fmt.wBitsPerSample != Waveform::K_BITS_PER_SAMPLE)
        throw std::runtime_error(
            "WavReader: поддерживается только 16 бит на отсчёт: " + path);

    // Чтение заголовка data
    DataChunkHeader dataHdr{};
    file.read(reinterpret_cast<char*>(&dataHdr), sizeof(dataHdr));
    if(!file)
        throw std::runtime_error(
            "WavReader: файл слишком короткий (data-заголовок): " + path);
    if(dataHdr.chunkId != ID_DATA)
        throw std::runtime_error("WavReader: отсутствует чанк data: " + path);

    // Чтение всех отсчетов
    const size_t NUM_SAMPLES = dataHdr.chunkSize / sizeof(int16_t);
    std::vector<int16_t> samples(NUM_SAMPLES);
    file.read(reinterpret_cast<char*>(samples.data()),
              static_cast<std::streamsize>(dataHdr.chunkSize));
    if(!file)
        throw std::runtime_error("WavReader: данные усечены (data-чанк): " +
                                 path);

    return Waveform(std::move(samples));
}

void WavWriter::write(const std::string& path, const Waveform& waveform) const
{
    std::ofstream file(path, std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("WavWriter: не удалось создать файл: " + path);

    const auto DATA_BYTES =
        static_cast<uint32_t>(waveform.numSamples() * sizeof(int16_t));

    // Формирование и запись RIFF-чанка
    RiffChunk riff{};
    riff.chunkId = ID_RIFF;
    riff.chunkSize =
        4 + sizeof(FmtChunk) + sizeof(DataChunkHeader) + DATA_BYTES;
    riff.waveId = ID_WAVE;
    file.write(reinterpret_cast<const char*>(&riff), sizeof(riff));

    // Формирование и запись fmt-чанк
    FmtChunk fmt{};
    fmt.chunkId = ID_FMT;
    fmt.chunkSize = 16;
    fmt.wFormatTag = 1;
    fmt.wChannels = Waveform::K_CHANNELS;
    fmt.dwSamplesPerSec = Waveform::K_SAMPLE_RATE;
    fmt.dwAvgBytesPerSec = Waveform::K_SAMPLE_RATE * Waveform::K_CHANNELS *
                           (Waveform::K_BITS_PER_SAMPLE / 8);
    fmt.wBlockAlign = Waveform::K_CHANNELS * (Waveform::K_BITS_PER_SAMPLE / 8);
    fmt.wBitsPerSample = Waveform::K_BITS_PER_SAMPLE;
    file.write(reinterpret_cast<const char*>(&fmt), sizeof(fmt));

    // Формирование и запись data-чанка
    DataChunkHeader dataHdr{};
    dataHdr.chunkId = ID_DATA;
    dataHdr.chunkSize = DATA_BYTES;
    file.write(reinterpret_cast<const char*>(&dataHdr), sizeof(dataHdr));

    // Запись всех отсчетов
    if(DATA_BYTES > 0)
        file.write(reinterpret_cast<const char*>(waveform.samples().data()),
                   static_cast<std::streamsize>(DATA_BYTES));

    if(!file)
        throw std::runtime_error("WavWriter: ошибка записи файла: " + path);
}

}  // namespace Wav
