#ifndef SOUND_PROCESSOR_WAVEFORM_H
#define SOUND_PROCESSOR_WAVEFORM_H

#include <cstdint>
#include <vector>

/**
 * @brief Объектная модель моно PCM-сигнала в оперативной памяти.
 *
 * Хранит отсчёты в виде 16-битных знаковых целых при фиксированной
 * частоте дискретизации 44100 Гц. Предоставляет вспомогательные методы
 * для перевода единиц времени (отсчёты ↔ мс / сек) и запроса длительности.
 */
class Waveform
{
public:
    static constexpr uint32_t K_SAMPLE_RATE =
        44100; /**< Частота дискретизации, Гц. */
    static constexpr uint16_t K_CHANNELS = 1; /**< Число каналов (моно). */
    static constexpr uint16_t K_BITS_PER_SAMPLE =
        16; /**< Число бит на отсчёт. */

    /** @brief Создаёт пустой звуковой фрагмент */
    Waveform() = default;

    /**
     * @brief Создаёт звуковой фрагмент из готового буфера отсчётов.
     * @param samples Буфер PCM-отсчётов; передаётся с захватом владения.
     */
    explicit Waveform(std::vector<int16_t> samples);

    Waveform(const Waveform&) = default;
    Waveform& operator=(const Waveform&) = default;
    Waveform(Waveform&&) noexcept = default;
    Waveform& operator=(Waveform&&) noexcept = default;
    ~Waveform() = default;

    /** @name Доступ к отсчётам */
    /** @{ */

    /** @brief Возвращает изменяемую ссылку на буфер отсчётов. */
    std::vector<int16_t>& samples() { return _samples; }

    /** @brief Возвращает константную ссылку на буфер отсчётов. */
    const std::vector<int16_t>& samples() const { return _samples; }

    /** @brief Возвращает общее число отсчётов. */
    size_t numSamples() const { return _samples.size(); }

    /** @brief Возвращает `true`, если фрагмент не содержит отсчётов. */
    bool empty() const { return _samples.empty(); }

    /** @} */

    /** @name Перевод единиц времени */
    /** @{ */

    /**
     * @brief Переводит миллисекунды в число отсчётов (округление вниз).
     * @param msec Время в миллисекундах.
     * @return Соответствующее число отсчётов.
     */
    size_t msToSamples(double msec) const;

    /**
     * @brief Переводит секунды в число отсчётов (округление вниз).
     * @param sec Время в секундах.
     * @return Соответствующее число отсчётов.
     */
    size_t secToSamples(double sec) const;

    /**
     * @brief Переводит число отсчётов в миллисекунды.
     * @param n Число отсчётов.
     * @return Длительность в миллисекундах.
     */
    double samplesToMs(size_t n) const;

    /**
     * @brief Переводит число отсчётов в секунды.
     * @param n Число отсчётов.
     * @return Длительность в секундах.
     */
    double samplesToSec(size_t n) const;

    /** @} */

    /** @name Длительность фрагмента */
    /** @{ */

    /** @brief Возвращает полную длительность фрагмента в миллисекундах. */
    double durationMs() const;

    /** @brief Возвращает полную длительность фрагмента в секундах. */
    double durationSec() const;

    /** @} */

private:
    std::vector<int16_t>
        _samples; /**< Буфер PCM-отсчётов, по одному на кадр. */
};

#endif  // SOUND_PROCESSOR_WAVEFORM_H
