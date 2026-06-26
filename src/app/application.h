#ifndef SOUND_PROCESSOR_APPLICATION_H
#define SOUND_PROCESSOR_APPLICATION_H

#include "converter.h"

/**
 * @brief Оркестратор приложения: связывает парсер, конвертер, ввод-вывод и
 * применение пайплайна.
 *
 * Этап конфигурирования (configure) регистрирует продюсеры фильтров и отделён
 * от решения основной задачи (start). Исключения наружу не подавляются —
 * их обрабатывает функция main.
 */
class Application
{
public:
    Application() = default;

    /**
     * @brief Конфигурирует приложение: регистрирует все стандартные продюсеры.
     */
    void configure();

    /**
     * @brief Выполняет основную работу: разбор аргументов, построение
     * пайплайна, чтение/генерацию сигнала, применение фильтров и запись
     * результата.
     * @param argc Число аргументов командной строки.
     * @param argv Массив аргументов командной строки.
     * @return Код возврата приложения.
     */
    int start(int argc, char* argv[]);

private:
    /** @brief Печатает справку по использованию приложения в stdout. */
    void printHelp() const;

    CmdLineArgs2PipelineConverter
        _converter; /**< Конвертер дескрипторов в пайплайн. */
};

#endif  // SOUND_PROCESSOR_APPLICATION_H
