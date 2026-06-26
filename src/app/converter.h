#ifndef SOUND_PROCESSOR_CONVERTER_H
#define SOUND_PROCESSOR_CONVERTER_H

#include "core/filter.h"
#include "core/filter_descriptor.h"
#include "core/pipeline.h"

#include <map>
#include <string>
#include <vector>

/**
 * @brief Тип продюсера фильтра.
 *
 * Указатель на функцию, которая по дескриптору фильтра создаёт конкретный
 * фильтр в куче и возвращает его через полиморфный базовый указатель IFilter*.
 */
using FilterProducer = IFilter* (*)(const FilterDescriptor&);

/**
 * @brief Конвертер: преобразует дескрипторы фильтров (из АКС) в пайплайн.
 */
class CmdLineArgs2PipelineConverter
{
public:
    /**
     * @brief Строит пайплайн по списку дескрипторов фильтров.
     * @param descriptors Дескрипторы, полученные от парсера.
     * @return Сконструированный пайплайн (по значению, с перемещением).
     * @throws std::invalid_argument Если фильтр неизвестен или его параметры
     *         некорректны.
     */
    Pipeline
    createPipeline(const std::vector<FilterDescriptor>& descriptors) const;

    /**
     * @brief Регистрирует продюсера для названия фильтра.
     * @param filterName Название фильтра (ключ).
     * @param producer   Функция-создатель фильтра.
     */
    void addFilterProducer(const std::string& filterName,
                           FilterProducer producer);

    /**
     * @brief Возвращает продюсера по названию фильтра.
     * @param filterName Название фильтра.
     * @return Указатель на функцию-продюсер или nullptr, если не найден.
     */
    FilterProducer getFilterProducer(const std::string& filterName) const;

private:
    std::map<std::string, FilterProducer>
        _producers; /**< Карта «имя → продюсер». */
};

#endif  // SOUND_PROCESSOR_CONVERTER_H
