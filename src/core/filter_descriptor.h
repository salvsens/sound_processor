#ifndef SOUND_PROCESSOR_FILTER_DESCRIPTOR_H
#define SOUND_PROCESSOR_FILTER_DESCRIPTOR_H

#include <string>
#include <vector>

/**
 * @brief Дескриптор фильтра — результат разбора одной группы «-f ...».
 *
 * Хранит название фильтра отдельно от его параметров. Параметры остаются
 * нераспарсенными текстовыми токенами.
 */
struct FilterDescriptor
{
    std::string name; /**< Название фильтра (первый токен после -f). */
    std::vector<std::string>
        params; /**< Параметры фильтра как текстовые токены. */
};

#endif  // SOUND_PROCESSOR_FILTER_DESCRIPTOR_H
