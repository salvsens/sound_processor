#ifndef SOUND_PROCESSOR_ARGS_PARSER_H
#define SOUND_PROCESSOR_ARGS_PARSER_H

#include "core/filter_descriptor.h"

#include <cstring>
#include <string>
#include <vector>

/**
 * @brief Парсер аргументов командной строки (ПАКС).
 *
 * Разбирает пары (argc, argv) по шаблону:
 * @code
 * {app} [-i in.wav] [-o out.wav] [-f name params...] [-f name params...] ...
 * @endcode
 * Проверяет синтаксис флагов и их комбинаций.
 */
class ArgsParser
{
public:
    /**
     * @brief Код результата разбора аргументов.
     */
    enum class Result
    {
        ok,     /**< Аргументы разобраны успешно. */
        noArgs, /**< Аргументы не переданы — нужно показать справку. */
        badArgs /**< Некорректная последовательность аргументов. */
    };

    ArgsParser() = default;

    /**
     * @brief Разбирает аргументы командной строки.
     * @param argc Число аргументов.
     * @param argv Массив аргументов.
     * @return Код результата разбора.
     */
    Result parse(int argc, char* argv[]);

    /** @brief Возвращает true, если задан входной файл (флаг -i). */
    bool hasInFileName() const { return _hasInFileName; }

    /** @brief Возвращает true, если задан выходной файл (флаг -o). */
    bool hasOutFileName() const { return _hasOutFileName; }

    /** @brief Возвращает имя входного файла (валидно при hasInFileName()). */
    const std::string& getInFileName() const { return _inFileName; }

    /** @brief Возвращает имя выходного файла (валидно при hasOutFileName()). */
    const std::string& getOutFileName() const { return _outFileName; }

    /** @brief Возвращает список разобранных дескрипторов фильтров. */
    const std::vector<FilterDescriptor>& getFilterDescriptors() const
    {
        return _filterDescriptors;
    }

private:
    /** @brief Сбрасывает состояние парсера перед новым разбором. */
    void reset();

    bool _hasInFileName = false;  /**< Был ли задан флаг -i. */
    bool _hasOutFileName = false; /**< Был ли задан флаг -o. */
    std::string _inFileName;      /**< Имя входного файла. */
    std::string _outFileName;     /**< Имя выходного файла. */
    std::vector<FilterDescriptor>
        _filterDescriptors; /**< Дескрипторы фильтров. */
};

#endif  // SOUND_PROCESSOR_ARGS_PARSER_H
