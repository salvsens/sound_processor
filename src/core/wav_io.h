#ifndef SOUND_PROCESSOR_WAV_IO_H
#define SOUND_PROCESSOR_WAV_IO_H

#include "waveform.h"

#include <string>

namespace Wav {

/**
 * @brief Вспомогательная функция: собирает четырёхбайтовый идентификатор чанка
 *        из символьных литералов в little-endian порядке.
 */
constexpr uint32_t makeId(char a, char b, char c, char d)  // NOLINT
{                                                          // NOLINT
    return static_cast<uint32_t>(static_cast<uint8_t>(a)) |
           static_cast<uint32_t>(static_cast<uint8_t>(b)) << 8 |
           static_cast<uint32_t>(static_cast<uint8_t>(c)) << 16 |
           static_cast<uint32_t>(static_cast<uint8_t>(d)) << 24;
}

static constexpr uint32_t ID_RIFF = makeId('R', 'I', 'F', 'F');
static constexpr uint32_t ID_WAVE = makeId('W', 'A', 'V', 'E');
static constexpr uint32_t ID_FMT = makeId('f', 'm', 't', ' ');
static constexpr uint32_t ID_DATA = makeId('d', 'a', 't', 'a');

/**
 * @brief Заголовок RIFF-контейнера (смещение 0x00, размер 12 байт).
 */
struct __attribute__((packed)) RiffChunk
{
    uint32_t chunkId;   /**< Сигнатура: "RIFF". */
    uint32_t chunkSize; /**< Размер файла минус 8 байт. */
    uint32_t waveId;    /**< Тип контейнера: "WAVE". */
};

/**
 * @brief Чанк описания формата (смещение 0x0C, размер 24 байта).
 */
struct __attribute__((packed)) FmtChunk
{
    uint32_t chunkId;    /**< Сигнатура: "fmt ". */
    uint32_t chunkSize;  /**< Размер тела чанка: 16 байт. */
    uint16_t wFormatTag; /**< Тип кодирования: 1 = PCM. */
    uint16_t wChannels;  /**< Число каналов: 1 (моно). */
    uint32_t dwSamplesPerSec; /**< Частота дискретизации: 44100 Гц. */
    uint32_t dwAvgBytesPerSec; /**< Байт в секунду: 88200. */
    uint16_t wBlockAlign;      /**< Размер блока: 2 байта. */
    uint16_t wBitsPerSample;   /**< Бит на отсчёт: 16. */
};

/**
 * @brief Заголовок чанка данных (смещение 0x24, без самих отсчётов).
 */
struct __attribute__((packed)) DataChunkHeader
{
    uint32_t chunkId; /**< Сигнатура: "data". */
    uint32_t chunkSize; /**< Размер данных в байтах (= numSamples * 2). */
};

/**
 * @brief Читает WAV-файл формата PCM 44100/mono/16bit и возвращает Waveform.
 *
 * Бросает `std::runtime_error` при любых проблемах с файлом или форматом,
 * но никогда не завершает программу аварийно.
 */
class WavReader
{
public:
    /**
     * @brief Открывает файл по пути @p path и загружает PCM-отсчёты.
     * @param path Путь к входному WAV-файлу.
     * @return Заполненный объект Waveform.
     * @throws std::runtime_error Если файл не открывается, имеет неверную
     *         сигнатуру, неподдерживаемый формат или усечённые данные.
     */
    Waveform read(const std::string& path) const;
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Записывает Waveform в WAV-файл формата PCM 44100/mono/16bit.
 *
 * Бросает `std::runtime_error`, если файл не удаётся создать или записать.
 */
class WavWriter
{
public:
    /**
     * @brief Записывает объект @p waveform в файл по пути @p path.
     * @param path     Путь к выходному WAV-файлу (создаётся или
     * перезаписывается).
     * @param waveform Звуковой фрагмент для записи.
     * @throws std::runtime_error Если файл не открывается или запись не
     * удалась.
     */
    void write(const std::string& path, const Waveform& waveform) const;
};

}  // namespace Wav

#endif  // SOUND_PROCESSOR_WAV_IO_H
