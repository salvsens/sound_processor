#include "application.h"

#include "app/args_parser.h"
#include "app/filter_producers.h"
#include "core/wav_io.h"
#include "core/waveform.h"

#include <iostream>
#include <stdexcept>

namespace {

constexpr int EXIT_OK = 0;
constexpr int EXIT_BAD_ARGS = 1;

}  // namespace

void Application::configure()
{
    _converter.addFilterProducer("ampl", FilterProducers::amplFilterCreator);
    _converter.addFilterProducer("normalize",
                                 FilterProducers::normalizeFilterCreator);
    _converter.addFilterProducer("silence",
                                 FilterProducers::silenceFilterCreator);
    _converter.addFilterProducer("timestretch",
                                 FilterProducers::timestretchFilterCreator);
    _converter.addFilterProducer("lowpass",
                                 FilterProducers::lowpassFilterCreator);
    _converter.addFilterProducer("generator",
                                 FilterProducers::generatorFilterCreator);
}

int Application::start(int argc, char* argv[])
{
    ArgsParser parser;
    const ArgsParser::Result RESULT = parser.parse(argc, argv);

    if(RESULT == ArgsParser::Result::noArgs)
    {
        printHelp();
        return EXIT_OK;
    }
    if(RESULT == ArgsParser::Result::badArgs)
    {
        std::cerr << "Ошибка: некорректные аргументы командной строки.\n\n";
        printHelp();
        return EXIT_BAD_ARGS;
    }

    // Построение пайплайна по дескрипторам фильтров.
    Pipeline pipeline =
        _converter.createPipeline(parser.getFilterDescriptors());

    // Получение исходного сигнала: из файла (-i) или пустой фрагмент.
    Waveform waveform;
    if(parser.hasInFileName())
    {
        Wav::WavReader reader;
        waveform = reader.read(parser.getInFileName());
    }

    // Последовательное применение фильтров.
    if(pipeline.apply(&waveform) != State::ok)
        throw std::runtime_error("Не удалось применить цепочку фильтров.");

    // Запись результата только при заданном выходном файле (-o).
    if(parser.hasOutFileName())
    {
        Wav::WavWriter writer;
        writer.write(parser.getOutFileName(), waveform);
    }

    return EXIT_OK;
}

void Application::printHelp() const
{
    std::cout << "Sound Processor — цифровая обработка WAV (PCM 44100 Гц, "
                 "моно, 16 бит)\n\n"
                 "Использование:\n"
                 "  sound_processor [-i input.wav] [-o output.wav] [-f имя "
                 "параметры...] ...\n\n"
                 "Опции:\n"
                 "  -i FILE   входной WAV-файл\n"
                 "  -o FILE   выходной WAV-файл\n"
                 "  -f ...    фильтр и его параметры (можно указать несколько "
                 "раз)\n\n"
                 "Фильтры:\n"
                 "  ampl factor\n"
                 "  normalize [peak]\n"
                 "  silence {sec|ms} start end\n"
                 "  timestretch factor\n"
                 "  lowpass window_size\n"
                 "  generator sin frequency_hz duration_ms\n"
                 "  generator am amplitude carrier_hz modulation_hz depth "
                 "duration_ms\n"
                 "  generator fm amplitude carrier_hz modulation_hz "
                 "deviation_hz duration_ms\n\n"
                 "Пример:\n"
                 "  sound_processor -i in.wav -o out.wav -f ampl 0.8 -f "
                 "timestretch 2\n";
}
