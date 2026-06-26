cmake_minimum_required(VERSION 3.16)
project(sound_processor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Catch2 (используется если XiCatch2WithMain не задан родительским проектом)
if(NOT TARGET XiCatch2WithMain)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
    add_library(XiCatch2WithMain INTERFACE)
    target_link_libraries(XiCatch2WithMain INTERFACE Catch2::Catch2WithMain)
endif()

set(CORE_SOURCES
    src/core/waveform.h
    src/core/waveform.cpp
    src/core/wav_io.h
    src/core/wav_io.cpp
    src/core/filter.h
    src/core/filter_descriptor.h
    src/core/pipeline.h
    src/core/pipeline.cpp
)

set(FILTER_SOURCES
    src/filters/transform_filters.h
    src/filters/transform_filters.cpp
    src/filters/generator_filters.h
    src/filters/generator_filters.cpp
)

set(APP_SOURCES
    src/app/args_parser.h
    src/app/args_parser.cpp
    src/app/converter.h
    src/app/converter.cpp
    src/app/filter_producers.h
    src/app/filter_producers.cpp
    src/app/application.h
    src/app/application.cpp
)

# Основной исполняемый файл
add_executable(sound_processor
    src/main.cpp
    ${CORE_SOURCES}
    ${FILTER_SOURCES}
    ${APP_SOURCES}
)

target_include_directories(sound_processor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Тесты
add_executable(test_sound_processor
    ${CORE_SOURCES}
    ${FILTER_SOURCES}
    ${APP_SOURCES}
    tests/test_waveform.cpp
    tests/test_wav_io.cpp
    tests/test_generator_filters.cpp
    tests/test_transform_filters.cpp
    tests/test_pipeline.cpp
    tests/test_args_parser.cpp
    tests/test_converter.cpp
)

target_include_directories(test_sound_processor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}   # для catch2/catch_shim.hpp
)
target_link_libraries(test_sound_processor PRIVATE XiCatch2WithMain)
