#ifndef SOUND_PROCESSOR_PIPELINE_H
#define SOUND_PROCESSOR_PIPELINE_H

#include "filter.h"

#include <vector>

/**
 * @brief Цепочка фильтров, последовательно применяемая к звуковому фрагменту.
 *
 * Пайплайн владеет своими фильтрами через полиморфные базовые указатели
 * IFilter* и удаляет их в деструкторе. Семантика копирования запрещена, чтобы
 * исключить double free; перемещение разрешено для эффективной
 * передачи объекта между функциями.
 */
class Pipeline
{
public:
    Pipeline() = default;

    /** @brief Запрет копирования: пайплайн — единоличный владелец фильтров. */
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    /** @brief Перемещающий конструктор: забирает фильтры у @p other. */
    Pipeline(Pipeline&& other) noexcept;

    /** @brief Перемещающее присваивание: освобождает свои фильтры и забирает
     * чужие. */
    Pipeline& operator=(Pipeline&& other) noexcept;

    /** @brief Удаляет все принадлежащие пайплайну фильтры. */
    ~Pipeline();

    /**
     * @brief Применяет все фильтры по порядку к звуковому фрагменту.
     * @param sound Указатель на обрабатываемый фрагмент.
     * @return State::ok, если все фильтры применились успешно; иначе
     * State::error (применение прекращается на первом сбойном фильтре).
     */
    State apply(Waveform* sound) const;

    /**
     * @brief Добавляет фильтр в конец цепочки, забирая владение им.
     * @param filter Указатель на созданный в куче фильтр (не nullptr).
     * @return Тот же указатель @p filter для удобства использования.
     */
    IFilter* addFilter(IFilter* filter);

    /** @brief Возвращает число фильтров в цепочке. */
    size_t getFilterNumbers() const { return _filters.size(); }

    /** @brief Возвращает указатель на фильтр с индексом @p ind (без проверки
     * границ). */
    IFilter* operator[](size_t ind) const { return _filters[ind]; }

private:
    /** @brief Освобождает все фильтры и очищает контейнер. */
    void clear();

    std::vector<IFilter*> _filters; /**< Принадлежащие пайплайну фильтры. */
};

#endif  // SOUND_PROCESSOR_PIPELINE_H
