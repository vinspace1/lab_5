cmake_minimum_required(VERSION 3.10)
project(lab_5 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск GoogleTest
find_package(GTest REQUIRED)

# Основной исполняемый файл
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/complex_type.cpp
    src/memory_resource.cpp
)

# Указываем директории с заголовками для основного проекта
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Файл с юнит-тестами
add_executable(${PROJECT_NAME}_tests
    tests/tests.cpp
    src/complex_type.cpp
    src/memory_resource.cpp
)

# Указываем директории с заголовками для тестов
target_include_directories(${PROJECT_NAME}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Линкуем GoogleTest
target_link_libraries(${PROJECT_NAME}_tests GTest::gtest GTest::gtest_main)

# Включаем тестирование
enable_testing()
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)

# Отключаем -Werror для тестов или используем более мягкие настройки
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
    target_compile_options(${PROJECT_NAME}_tests PRIVATE -Wall -Wextra)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    target_compile_options(${PROJECT_NAME}_tests PRIVATE /W4)
endif()